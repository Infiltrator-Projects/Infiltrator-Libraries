# Temporal provider architecture

## Status

This document records the target architecture for extensible calendar and clock systems across the Infiltrator project family. It is a design goal and migration contract, not a claim that the current implementation already satisfies it.

The current implementation still contains compiled-in calendar and clock catalogues and grouped implementation files. Those are transitional implementation details to be replaced deliberately rather than treated as the long-term extension model.

## Goal

Calendars and clock systems are runtime-discovered providers.

Adding a calendar or clock must not require editing System Settings, extending a central switch statement or rebuilding a master catalogue merely to make the new system visible. A provider should be installable into a defined provider location, discovered on the next provider scan/startup, validated, and then exposed through the shared temporal registry.

Conceptually:

```text
provider bundle installed
        ↓
Common discovers manifest
        ↓
ABI / identity / trust validation
        ↓
provider loaded or connected
        ↓
calendar and/or clock descriptors registered
        ↓
System Settings and other consumers enumerate the registry
```

The architecture deliberately separates discovery from implementation. A provider may be implemented in C or C++ internally, but the supported native binary boundary is a small versioned C ABI.

## Provider as the unit of extension

A temporal provider is a self-contained extension unit that may supply:

- one calendar;
- one clock/time system; or
- a historically coherent pair or family of related calendar and clock systems.

A provider owns the domain-specific rules for the systems it supplies. Common owns the generic discovery, loading, ABI, metadata validation and registry mechanics. System Settings owns user selection and policy persistence. Calendar and other applications consume the registered capabilities rather than maintaining independent catalogues.

A typical source/provider bundle should keep implementation, tests and documentation together conceptually:

```text
providers/
  egyptian-early/
    provider.json
    README.md
    egyptian-early.c
    egyptian-early.h
    test-egyptian-early.c
```

Installed native artifacts may use platform packaging conventions, for example a manifest plus `.so` on Linux or manifest plus `.dll` on Windows. Source layout and installed layout do not have to be byte-for-byte identical, but the provider identity must remain stable.

## One provider, one authority

Provider-specific chronology or timekeeping rules must not be duplicated in System Settings, Common registry tables or unrelated application code.

The provider is the authority for:

- its stable system identifiers;
- conversion/calculation rules;
- domain-specific formatting rules;
- supported range and failure states;
- required geographic/astronomical inputs;
- historical assumptions and evidence decisions;
- validation vectors and boundary cases.

Common may provide shared mathematical and astronomical primitives, but those primitives must not silently become a second copy of a provider's historical policy.

## Per-system documentation

Every non-trivial calendar or clock supplied by a provider requires maintained technical documentation. Where a provider supplies multiple systems, each system must still have an individually identifiable specification, even if the provider uses one bundle README plus linked system documents.

The documentation must record, as applicable:

- system name and stable identifier;
- historical/technical basis and sources;
- epoch, anchor or observational basis;
- year/month/day or day/hour structure;
- boundary conventions;
- geographic assumptions;
- algorithms and continuation rules;
- known uncertainty or disputed evidence;
- deliberate project decisions where evidence is incomplete;
- supported range;
- unavailable/failure states;
- representative examples;
- validation evidence;
- conditions under which the model should be revised.

Historical models are the project's current best supported model. They do not need to be labelled `reconstruction` in their user-facing names merely because scholarship may later improve. If stronger evidence changes the best model, the implementation and its documentation change together.

## Discovery

Common owns one runtime registry built from discovered providers. Consumers enumerate that registry.

The design must support at least:

- trusted system-wide provider locations;
- deterministic duplicate-ID handling;
- explicit ABI-version compatibility checks;
- manifest validation before executable code is entered;
- failure isolation so one broken optional provider does not disable unrelated providers;
- diagnostics explaining why a discovered provider was rejected;
- stable persisted IDs so a user selection survives provider upgrades;
- a defined outcome when a previously selected provider is absent.

Provider discovery must not depend on array order in a compiled table.

The exact installation paths are platform packaging details and may differ between Linux and Windows. Their semantics are stable even if the concrete directories evolve.

## Manifest

A provider has bounded static metadata that can be inspected before loading executable code. The final schema may evolve before ABI v1 is frozen, but it must cover at least:

- provider identity;
- provider version;
- required temporal-provider ABI version;
- supplied calendar IDs;
- supplied clock IDs;
- human-readable names/descriptions or localisation keys;
- capability flags;
- executable/library entry artifact;
- trust/source information where required by the host.

Illustrative only:

```json
{
  "id": "egyptian-early",
  "version": "1.0",
  "abi": 1,
  "provides": {
    "calendars": ["egyptian-early-lunar"],
    "clocks": ["egyptian-stellar-solar"]
  },
  "library": "libinfiltrator-temporal-egyptian-early.so"
}
```

The manifest is discovery metadata, not a place to reimplement the calendar or clock in declarative fragments.

## ABI

The public native provider boundary is C even when provider internals use C++.

The ABI should expose an entry point conceptually equivalent to:

```c
const InfiltratrTemporalProvider *
infiltratr_temporal_provider_get(void);
```

The returned descriptor is versioned and size-tagged. It describes the provider and exposes operation tables for the capabilities it supplies.

Calendar operations may include conversion between canonical instants/day coordinates and provider fields, formatting, navigation and validation. Clock operations may include formatting, state/boundary calculation and next-change scheduling. Exact operation sets are frozen only after the first real providers prove the interface.

No C++ standard-library object, exception, compiler-specific class layout, GTK object or Windows UI object crosses this ABI.

## Runtime registry

The runtime registry is the authority for what temporal capabilities are available in the current installation.

System Settings should therefore behave conceptually as:

```text
for each discovered calendar:
    show calendar in selector

for each discovered clock:
    show clock in selector
```

System Settings must not maintain a second hand-written list of calendars or clocks.

Applications that need temporal presentation consume the same provider identities/policy through the shared temporal contract. They must not infer availability from a list compiled into their own binaries.

## Trust and isolation

Discovery and trust are separate concerns.

A native shared library executes code with the authority of its host process. Therefore the existence of a discoverable provider file is not, by itself, sufficient reason to `dlopen`/`LoadLibrary` arbitrary untrusted user code into System Settings or another long-lived privileged-adjacent process.

The architecture must support the Amiga-like operational goal — install a provider and have the system discover it — without pretending that an in-process dynamic-library boundary is a sandbox.

The initial implementation may admit trusted, packaged providers from system-owned locations in process. If user-installed or third-party native providers are supported, their execution trust/isolation model must be defined explicitly before they are enabled. A dedicated provider host process is one acceptable direction; the exact mechanism is deliberately not frozen here.

Trust policy must not reintroduce a hard-coded capability catalogue. A provider may be discovered and reported as unavailable/untrusted without being executed.

## Failure model

A provider failure must remain local to that provider wherever practical.

At minimum:

- malformed manifests are rejected without executing the provider;
- unsupported ABI versions are rejected explicitly;
- duplicate stable IDs are not silently overwritten;
- initialization failure does not remove unrelated providers;
- provider output is validated at the ABI boundary;
- absence of an optional provider does not corrupt stored canonical timestamps;
- when a selected provider disappears, policy preserves its stable ID or records an explicit unavailable state rather than silently selecting a different historical system.

## Relationship to canonical data

Temporal providers are presentation/chronology capabilities, not storage-time mutators.

Canonical timestamps and durations remain in their authoritative machine representations. Selecting an Egyptian, Roman, decimal or other historical system changes how appropriate human-visible temporal values are interpreted or rendered; it does not rewrite filesystem timestamps, event instants, protocol values or monotonic durations.

## Migration from the current implementation

The current compiled catalogues in Common and Calendar remain valid implementation until replaced. Migration should be incremental and behaviour-preserving:

1. define and test the provider descriptor/ABI with real first-party providers;
2. move one existing calendar and one existing clock behind the provider contract;
3. make the runtime registry enumerate discovered providers;
4. change System Settings to consume only that registry;
5. migrate remaining built-in systems provider by provider;
6. remove central hard-coded capability catalogues once no longer authoritative;
7. add qualification ensuring a newly installed provider appears without editing System Settings or a central provider list.

The first Egyptian pair is an appropriate proving case because it is new work rather than compatibility-sensitive legacy behaviour:

- `egyptian-early-lunar` — Early Egyptian Lunar Calendar;
- `egyptian-stellar-solar` — Early Egyptian Stellar–Solar Time.

Their implementation should establish the per-provider source, documentation, test and discovery pattern that later temporal systems follow.

## Non-goals

This design does not require:

- a general-purpose application plugin framework;
- arbitrary scripting engines;
- loading every provider at shell startup before it is needed;
- duplicating provider-specific rules in Common;
- combining calendar and clock choice into one inseparable profile;
- allowing untrusted native code to execute in process merely because it was discovered.

Calendar and clock remain independently selectable capabilities even when one provider package happens to supply both.

## Completion criteria

The runtime-provider design is complete only when:

- providers are discovered from defined installation locations;
- provider ABI/version validation is enforced;
- at least one calendar and one clock are supplied without central catalogue edits;
- System Settings populates selectors from the runtime registry;
- applications consume the same stable IDs;
- provider removal/unavailability is handled explicitly;
- tests prove duplicate, malformed, incompatible and failing-provider behaviour;
- each shipped non-trivial temporal system has maintained implementation documentation and validation evidence;
- adding a conforming provider does not require a source change to System Settings or the central registry implementation.
