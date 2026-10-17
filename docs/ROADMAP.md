# Roadmap

This roadmap describes direction rather than a promise of dates. The exact released capability is defined by the tagged source and tests.

## Current foundation

- maintain the published ABI and exact consumer contract;
- keep shared primitives at least as strong as the best consumer implementation;
- qualify Linux, Windows and Apple consumers through CI;
- keep toolkit-neutral POSIX user paths and directory creation in Common rather than duplicating them in graphical consumers.

## Near-term direction

- promote reusable behaviour only after it has proved useful in more than one consumer;
- continue removing private duplicate helpers from consumers when the Common contract is stronger;
- expand regression coverage whenever a shared primitive gains a new caller;
- define and qualify the runtime temporal-provider manifest, versioned C ABI, discovery registry and failure/trust rules described in `TEMPORAL_PROVIDERS.md`;
- prove that provider model with one real calendar and one real clock before freezing ABI v1;
- migrate System Settings away from any compiled temporal capability list so its selectors enumerate the runtime registry.

## Longer-term direction

- add new shared capability families only where a stable product-neutral contract exists;
- preserve backward compatibility or introduce explicit versioned ABI transitions when incompatibility is unavoidable;
- migrate remaining built-in calendar and clock systems behind the temporal-provider boundary and remove central hard-coded catalogues once they are no longer authoritative;
- support installable temporal extensions without conflating provider discovery with permission to execute untrusted native code in process.

## Admission rule

New work enters Common only when it has a stable product-neutral contract and centralising it is at least as correct and maintainable as the best existing implementation. Project-specific behaviour stays with its owner.

## Completion rule

A roadmap item is complete only when implementation, automated tests, documentation and consumer integration agree. Publication of a Common release is not used to conceal unqualified behaviour.
