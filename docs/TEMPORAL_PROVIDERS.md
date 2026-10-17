# Filesystem-discovered temporal resources

## Status

This document records the target architecture for extensible calendar and clock systems across the Infiltrator project family. It is a design goal and migration contract, not a claim that the current implementation already satisfies it.

The current implementation still contains compiled-in calendar and clock catalogues and grouped implementation files. Those are transitional implementation details. The long-term model is deliberately closer to an Amiga Workbench-style resource directory: the installed directory is the catalogue.

## Governing concept

A calendar or clock exists because a self-contained temporal resource has been installed in a recognised directory, not because its identifier was compiled into a master list.

Conceptually:

```text
Temporal/Calendars/
    Gregorian
    Coptic
    Egyptian-Early-Lunar

Temporal/Clocks/
    Standard-24
    Roman-Temporal
    Egyptian-Stellar-Solar
```

Drop a valid calendar resource into the calendar directory and, after discovery/reload, the system knows that calendar exists. Drop a valid clock resource into the clock directory and the system knows that clock exists. Remove the resource and that capability is no longer available.

No source edit to System Settings, Calendar or a central registry is required merely to add the capability.

This is the invariant:

> The filesystem namespace is the installed temporal capability catalogue.

## Resource representation is deliberately not frozen

This design does **not** yet decide what the physical resource must be.

A resource might ultimately be:

- compiled object/module code produced elsewhere and copied into the directory;
- a shared library such as `.so` or `.dll`;
- a declarative data file interpreted by a common engine;
- a directory/bundle containing metadata, documentation and implementation;
- another compact native format that proves cleaner during implementation.

The representation is an engineering detail to be decided after prototyping real calendars and clocks. The architecture must not accidentally turn today's preferred file format into the governing idea.

What is fixed is the behaviour: the resource is self-identifying enough for the system to discover it, determine what capability it supplies, validate whether it can be used, and expose it through the common temporal registry without a compiled master list.

If executable native code is used, it needs an explicit compatibility contract. If data is used, it needs an explicit bounded schema. Those details are subordinate to the drop-in resource model.

## One resource, one system authority

Each installed calendar or clock is independently identifiable and owns the rules specific to that system.

The resource is the authority for, as applicable:

- stable system identifier;
- display identity;
- conversion/calculation rules;
- domain-specific formatting rules;
- supported range and unavailable states;
- required geographic/astronomical inputs;
- historical assumptions and evidence decisions;
- validation vectors and boundary cases.

Shared arithmetic, astronomy, localisation and loading primitives may be provided by Common, but provider-specific historical policy must not be duplicated into System Settings or a central catalogue.

A distribution package may install several resources together, but calendar and clock identities remain independent. For example, an Egyptian package can install one calendar resource and one clock resource without making them one inseparable setting.

## Per-system documentation

Every non-trivial calendar and clock has its own maintained technical document associated directly with that resource.

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

## Discovery

Common owns the generic discovery mechanism and exposes the resulting temporal registry to the rest of the system.

Conceptually:

```text
scan known calendar/clock directories
        ↓
inspect each child resource
        ↓
identify and validate it
        ↓
make valid capabilities available
        ↓
System Settings and applications enumerate what is installed
```

There is no compiled master enumeration of calendar IDs or clock IDs.

The scanner must provide deterministic handling for duplicate IDs, malformed resources, incompatible resource versions and failed implementations. One bad resource must not prevent unrelated resources from being discovered.

The exact physical roots are platform packaging details. System-wide and, if safely supported, per-user roots may both exist. Their precedence and duplicate rules must be explicit.

## Runtime registry

The runtime registry is derived from directory contents. It is not the authority itself.

System Settings therefore behaves conceptually as:

```text
read discovered calendars → populate Calendar selector
read discovered clocks    → populate Clock selector
```

Other applications resolve the same stable IDs through the shared temporal layer. They do not maintain private copies of the installed list.

A cache/index may be used for performance, but it is derived state and must be rebuildable from the resource directories.

## Change detection

The minimum contract is discovery at process/session startup. The design should also permit explicit rescan and, where practical, filesystem change notification so newly installed or removed resources can become visible without rebooting the machine.

Live hot-plug behaviour is desirable but not required to define the architecture. Correct discovery from the directory is the invariant.

## Missing resources

Persisted user policy stores stable resource IDs.

If a selected resource is removed, the system reports that selection as unavailable. It must not silently reinterpret the setting as another calendar or clock. Reinstalling the same stable resource ID can make the original selection usable again.

Canonical timestamps and durations remain unchanged throughout.

## Trust and executable resources

If the chosen resource representation is purely data, it is parsed as bounded external input.

If a resource contains executable native code, that code executes with the authority of its host and therefore needs an explicit trust and compatibility model. Merely appearing in a directory does not automatically make arbitrary untrusted code safe to execute in-process.

The trust solution must preserve the drop-in resource concept without reintroducing a hard-coded capability list. The exact mechanism is intentionally left open until the implementation format is chosen.

## Relationship to canonical data

Temporal resources provide chronology and human presentation. They do not rewrite canonical machine data.

Selecting Egyptian, Roman, Coptic, decimal or another system changes how appropriate human-visible dates/times are calculated or displayed. Filesystem timestamps, event instants, protocol timestamps, sort keys and monotonic durations remain in their canonical forms.

## Migration from the current implementation

Migration should preserve current behaviour while changing the source of capability discovery:

1. define the resource-directory contract and stable resource identity;
2. prototype at least one real calendar and one real clock as drop-in resources;
3. use those prototypes to decide the cleanest physical representation and compatibility contract;
4. implement directory discovery and a derived runtime registry in Common;
5. make System Settings populate selectors only from discovered resources;
6. migrate existing systems incrementally into independent resources;
7. remove central compiled capability catalogues when they are no longer authoritative;
8. qualify the defining behaviour: adding a valid resource to the directory makes it available without editing System Settings or a master registry.

The first Egyptian pair is an appropriate proving case:

- `egyptian-early-lunar` — Early Egyptian Lunar Calendar;
- `egyptian-stellar-solar` — Early Egyptian Stellar–Solar Time.

They should establish the resource/documentation/test/discovery pattern while also helping us determine whether the eventual resource representation should be compiled, declarative or hybrid.

## Non-goals

This design does not require:

- choosing the final on-disk format before prototyping;
- compiling every calendar or clock into Common;
- editing a master list whenever a resource is added;
- a general-purpose application plugin framework;
- making calendar and clock one inseparable profile;
- treating a cache or registry file as the real source of installed capability.

## Completion criteria

The design is complete when:

- defined temporal directories are the installed capability authority;
- the runtime registry is derived from those directories;
- at least one calendar and one clock can be added without central source edits;
- System Settings selectors enumerate discovered resources;
- removal/unavailability is explicit and reversible by reinstalling the resource;
- malformed or failing resources do not break unrelated capabilities;
- each shipped non-trivial calendar or clock has its own maintained technical documentation and validation evidence;
- the chosen representation has been selected from evidence gathered by real provider prototypes rather than assumed in advance.
