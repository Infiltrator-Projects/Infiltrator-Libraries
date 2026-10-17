# Filesystem-discovered temporal resources

## Status

This document records the target architecture for extensible calendar and clock systems across the Infiltrator project family. It is a design goal and migration contract, not a claim that the current implementation already satisfies it.

The current implementation still contains compiled-in calendar and clock catalogues and grouped implementation files. Those are transitional implementation details. The long-term model is deliberately closer to an Amiga Workbench-style resource directory: the installed directory is the catalogue.

## Core idea

A calendar or clock exists because an installed temporal resource exists in a known directory, not because its identifier was compiled into a master list.

Conceptually:

```text
Temporal/Calendars/
    Gregorian.calendar/
    Coptic.calendar/
    Egyptian-Early-Lunar.calendar/

Temporal/Clocks/
    Standard-24.clock/
    Roman-Temporal.clock/
    Egyptian-Stellar-Solar.clock/
```

Drop a valid resource into the appropriate directory and, after the next scan/reload, the whole system can discover it. Remove it and the capability is no longer available. No source change to System Settings, Calendar or a central registry is required merely to add the name to the system.

This is the governing design principle:

> The filesystem namespace is the installed capability catalogue.

## A resource is not necessarily compiled code

A temporal resource is a self-describing installed object. It is not synonymous with a C/C++ plugin.

Where a calendar or clock can be expressed faithfully as data, the resource should be data-driven and require no provider binary at all. Examples may include fixed epochs, fixed month structures, names, arithmetic leap rules, display patterns or other behaviour supported by the shared temporal engine.

Where a system genuinely requires specialised computation that the generic engine cannot express cleanly, the resource may carry an executable provider module compiled separately and dropped into the same resource bundle. On Linux this may be a `.so`; on Windows a `.dll`. Its public binary boundary is a small versioned C ABI even if its internals are C++.

The architecture therefore supports both:

```text
pure resource
    descriptor + model data + documentation

executable resource
    descriptor + documentation + optional model data + provider.so/provider.dll
```

Compiled code is an escape hatch for genuine algorithmic requirements, not the definition of the extension system.

## Resource bundle

Each installed calendar or clock is independently identifiable. A directory-style bundle is preferred because it can carry the complete definition and its documentation together.

Illustrative layout:

```text
Temporal/Calendars/Egyptian-Early-Lunar.calendar/
    resource.json
    model.json
    README.md

Temporal/Clocks/Egyptian-Stellar-Solar.clock/
    resource.json
    README.md
    provider.so          # only if required
```

The exact filenames and schema are not frozen yet. The important contract is that each directory entry is self-describing and can be admitted without editing a central capability list.

A package may install several independent resource bundles, but calendar and clock identities remain independent. A historical Egyptian package could therefore install one `.calendar` resource and one `.clock` resource without turning them into one inseparable setting.

## Discovery

Common owns the generic scanner/parser and exposes the resulting temporal resource registry.

Discovery is conceptually:

```text
scan known calendar and clock directories
        ↓
inspect each child resource
        ↓
validate descriptor and stable identity
        ↓
load declarative model OR bind optional executable implementation
        ↓
add valid capability to runtime registry
        ↓
consumers enumerate what is actually installed
```

There is no compiled master enumeration of calendar IDs or clock IDs.

The scanner must provide deterministic handling for duplicate IDs, malformed resources, incompatible resource/ABI versions and unavailable executable components. One bad resource must not prevent unrelated resources from loading.

The exact physical roots are platform-specific packaging details. System-wide and, if safely supported, per-user roots may both exist. Their precedence and duplicate rules must be explicit.

## Descriptor

Every resource has bounded metadata that can be read before any optional executable code is loaded.

At minimum it identifies:

- stable resource ID;
- resource kind: calendar or clock;
- human-readable name or localisation key;
- resource format version;
- implementation kind: declarative or executable;
- capabilities and required inputs;
- optional executable artifact and ABI version when one is required.

Illustrative only:

```json
{
  "id": "egyptian-early-lunar",
  "kind": "calendar",
  "name": "Early Egyptian Lunar Calendar",
  "format": 1,
  "implementation": "declarative",
  "model": "model.json"
}
```

or, when custom code is genuinely necessary:

```json
{
  "id": "egyptian-stellar-solar",
  "kind": "clock",
  "name": "Early Egyptian Stellar–Solar Time",
  "format": 1,
  "implementation": "native",
  "abi": 1,
  "library": "provider.so"
}
```

The descriptor describes the resource. It does not recreate a second central catalogue.

## Generic engine versus specialised implementation

Common should provide a strong generic temporal engine capable of expressing the common building blocks used by many systems: epochs, cycles, arithmetic month/year structures, names, boundaries, fixed subdivisions and shared astronomical primitives where appropriate.

A resource chooses the simplest faithful representation:

1. declarative model when the generic engine is sufficient;
2. native provider only when the system's rules genuinely require code.

Do not force a historically complex system into an unreadable mini-language merely to avoid a small native module. Equally, do not compile trivial tables and arithmetic rules into bespoke binaries when data is clearer and safer.

## Per-system documentation

Each calendar and clock resource carries or is directly associated with its own maintained technical document. Documentation is part of the resource's engineering identity, not an unrelated global appendix.

The document records, as applicable:

- stable ID and display name;
- historical/technical basis and sources;
- epoch, anchor or observational basis;
- year/month/day or day/hour structure;
- boundary conventions;
- geographic assumptions;
- algorithms and continuation rules;
- known uncertainty or disputed evidence;
- project decisions where evidence is incomplete;
- supported range;
- unavailable/failure states;
- representative examples;
- validation evidence;
- conditions under which the model should be revised.

Historical models represent the project's current best supported model. Their user-facing names do not need to say `reconstruction`. If stronger evidence changes the best model, the resource definition, tests and documentation change together.

## Runtime registry

The registry is built from the directory contents. It is not itself the source of installed capability.

System Settings therefore behaves conceptually as:

```text
read discovered calendars → populate Calendar selector
read discovered clocks    → populate Clock selector
```

Other applications resolve the same stable IDs through Common. They do not keep private copies of the installed list.

A cache/index may be used for performance, but it is derived state and can always be rebuilt from the resource directories. It must never become a shadow authority.

## Change detection

The minimum contract is scan at process/session startup. The design should also permit an explicit rescan and, where practical, filesystem change notification so installing/removing a resource can become visible without rebooting the machine.

Live hot-plug behaviour is desirable but is not required to define the architecture. Correct discovery from the resource directory is the invariant.

## Missing resources

Persisted user policy stores stable resource IDs.

If a selected resource is removed, the system reports that selection as unavailable. It must not silently reinterpret the setting as some other calendar or clock merely because another resource is present.

The canonical timestamp/data remains unchanged. Reinstalling the same stable resource ID can make the selection usable again.

## Trust and executable resources

Declarative resources are parsed as untrusted data and must be bounded/validated accordingly.

Native executable resources are different: a `.so` or `.dll` executes with the host's authority. Merely appearing in a directory must not accidentally turn arbitrary untrusted code into in-process System Settings code.

Discovery and execution permission are therefore separate. The system can see that a native resource exists, validate its descriptor and then either admit it under the configured trust policy or report it unavailable/untrusted.

The trust model must preserve the drop-in resource concept without reintroducing a hard-coded list. Trusted/system-installed native modules may be loaded in process; a future arbitrary-user/third-party native model may require an isolated provider host. The exact isolation mechanism is not frozen here.

## Optional native ABI

When a resource cannot be expressed by the generic engine, its optional native implementation exposes a versioned C ABI. A provider may use C or C++ internally.

Conceptually:

```c
const InfiltratrTemporalResourceImplementation *
infiltratr_temporal_resource_get(void);
```

The returned descriptor is versioned and size-tagged and supplies only the operations that the resource type requires. No C++ STL object, exception, GTK object or Windows UI object crosses the ABI.

The ABI is subordinate to the filesystem resource model: native code is one possible implementation carried by a resource, not the mechanism by which the system learns that the resource exists.

## Relationship to canonical data

Temporal resources provide chronology and human presentation. They do not rewrite canonical machine data.

Selecting Egyptian, Roman, Coptic, decimal or another system changes how appropriate human-visible dates/times are calculated or displayed. Filesystem timestamps, event instants, protocol timestamps, sort keys and monotonic durations remain in their canonical forms.

## Migration from the current implementation

Migration should preserve current behaviour while changing the source of capability discovery:

1. define the resource directory layout and descriptor format;
2. implement bounded directory scanning and a runtime registry in Common;
3. implement the generic declarative calendar/clock engine strongly enough for simple existing systems;
4. prove optional native loading with one system that genuinely needs it, rather than making every resource executable;
5. make System Settings populate selectors solely from discovered resources;
6. package existing systems as individual resources, migrating them incrementally;
7. remove central compiled capability catalogues when no longer authoritative;
8. qualify the defining behaviour: dropping a valid new resource into the directory makes it available without editing System Settings or a master registry.

The first Egyptian pair is an appropriate proving case:

- `egyptian-early-lunar` — Early Egyptian Lunar Calendar;
- `egyptian-stellar-solar` — Early Egyptian Stellar–Solar Time.

They should establish the resource, documentation, test and discovery pattern used by later systems.

## Non-goals

This design does not require:

- compiling every calendar or clock into Common;
- editing a master list whenever a resource is added;
- a general-purpose application plugin framework;
- a programming language embedded into every descriptor;
- making calendar and clock one inseparable profile;
- loading every native implementation before it is needed;
- granting arbitrary untrusted binaries execution merely because they were copied into a folder.

## Completion criteria

The design is complete when:

- defined temporal directories are the installed capability authority;
- the runtime registry is rebuilt from those directories;
- at least one declarative resource works without bespoke compiled code;
- optional native resources can be loaded through a versioned ABI where genuinely necessary;
- System Settings selectors enumerate discovered resources;
- adding a valid resource requires no source edit to System Settings or a central capability list;
- removal/unavailability is explicit and reversible by reinstalling the resource;
- malformed or failing resources do not break unrelated capabilities;
- each shipped non-trivial calendar or clock has its own maintained technical documentation and validation evidence.
