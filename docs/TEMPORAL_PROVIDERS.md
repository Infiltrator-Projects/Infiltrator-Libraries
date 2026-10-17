# Filesystem-discovered temporal resources

## Status

This is the design direction for installable calendar and clock systems. It is intentionally small at this stage.

The existing compiled calendar and clock catalogues remain in place. Nothing in this design removes or replaces them yet.

## Concept

A future temporal system should be installable by placing a self-contained resource in a recognised directory. The directory contents, rather than a compiled master list, tell the system what extra capabilities are present.

Conceptually:

```text
Temporal/
  calendars/
    ...installed calendar resources...
  clocks/
    ...installed clock resources...
```

Adding a valid resource should eventually make it available system-wide without editing System Settings, Calendar or a central list. Removing it should make that capability unavailable without changing canonical timestamps or other stored data.

## Current implementation step

Common now provides additive filesystem discovery for this model through `infiltratr/temporal_resource.h`.

The scanner:

- enumerates direct children of `calendars/` and `clocks/`;
- accepts ordinary files or directories as candidate resources;
- returns them in deterministic name order;
- ignores hidden entries, symbolic links/reparse points and unrelated filesystem objects;
- treats a missing resource directory as an empty catalogue;
- does not change the existing built-in temporal catalogue.

This is candidate discovery only. It deliberately does **not** yet parse, execute or register a calendar/clock implementation.

## Deliberately undecided

The physical resource format is not chosen yet. A resource may eventually be compiled code, a shared library, declarative data, a directory bundle, or a hybrid. We will choose that after implementing a real calendar and clock and seeing what the cleanest contract actually requires.

Likewise, no final provider ABI, manifest schema, trust model or hot-reload contract is frozen by this first step.

## Design rules

- The filesystem directory is the future installed-capability catalogue.
- Existing built-in calendars and clocks continue to work during migration.
- Calendar and clock remain independently selectable capabilities.
- Each non-trivial temporal system should have its own maintained technical/historical document.
- A missing resource must never silently become a different calendar or clock.
- One bad optional resource must not invalidate unrelated temporal systems.
- Canonical timestamps and durations are never rewritten merely because presentation resources change.

## First proving case

The first Egyptian pair is the intended real-world prototype:

- `egyptian-early-lunar` — Early Egyptian Lunar Calendar;
- `egyptian-stellar-solar` — Early Egyptian Stellar–Solar Time.

Those two systems should tell us what the resource payload actually needs. Only then should we freeze the executable/data representation.
