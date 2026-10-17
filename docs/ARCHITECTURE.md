# Architecture

## Purpose

Common is the canonical first-party shared library for portable mechanics, algorithms, design tokens, localisation, durable I/O and other behaviour that has proved genuinely reusable across the Infiltrator project family.

## Architectural model

The repository is intentionally layered:

- Portable core — dependency-light C interfaces for algorithms and data contracts that are meaningful across operating systems.
- Platform adapters — POSIX, dynamic-library and other host-facing code that translates platform mechanisms into stable Common contracts, including user/XDG path discovery, recursive directory creation and normalized absolute clock-deadline conversion where products would otherwise duplicate platform arithmetic or depend on toolkit helpers.
- Shared design/localisation — product-neutral visual tokens and localisation mechanics consumed by native and web-facing projects.
- Build and package surfaces — Make, CMake and installed-package metadata that let consumers use one reviewed implementation rather than private copies.
- Regression and consumer tests — contract tests that protect behaviour at the point where multiple projects now depend on it.

Common owns product-neutral behaviour only. Filesystem semantics remain in InfiltratorFS/Defragmenter, automotive semantics remain in Link and its product faces, and application-specific presentation remains with each application.

## Runtime temporal providers

Calendars and clock systems are moving to a runtime-discovered provider architecture. Common owns the generic provider manifest contract, versioned C ABI, discovery/validation mechanics and runtime registry. Provider-specific chronology and historical timekeeping rules remain with the provider implementation that supplies them.

Consumers do not own independent hard-coded lists of available temporal systems. System Settings enumerates the runtime registry for user selection; Calendar and other applications resolve the same stable provider identifiers when they need chronology or time presentation.

The current compiled calendar/clock catalogues are transitional implementation, not the intended extension boundary. The target design is that installing a conforming provider makes its capabilities discoverable without editing System Settings or extending a central compiled provider list.

Discovery does not imply trust. Native provider code executes with its host's process authority, so manifest discovery, trust policy and executable loading are separate stages. Trusted packaged providers may be loaded in process; any user-installed/third-party native provider model must define an explicit isolation/trust policy before such code is enabled.

The complete design and migration contract is maintained in [TEMPORAL_PROVIDERS.md](TEMPORAL_PROVIDERS.md).

## Ownership and dependency rule

The portable core is intentionally dependency-light. Consumers pin an exact reviewed Common revision; external libraries or platform APIs sit behind narrow adapters rather than defining Common semantics.

A function does not belong in Common merely because two files happen to look similar. Promotion requires a stable contract, clear ownership, tests that define edge behaviour, and evidence that centralising the implementation improves the project family rather than weakening the strongest caller.

## Source of truth

Public headers, implementation, tests and the exact tagged release together define Common behaviour. Documentation explains those contracts but does not override code or tests. USAGE.md defines consumer boundaries, while the design documents define presentation ownership.

## Change discipline

Shared code has a larger blast radius than product-local code. Changes therefore preserve ABI/API contracts where promised, add regression coverage for newly shared behaviour and avoid moving domain policy into the library. Consumers pin exact reviewed revisions so later Common development cannot silently change already-reviewed products.

## Related documentation

- USAGE.md
- design/README.md
- design/CONSUMERS.md
- docs/I18N.md
- docs/TEMPORAL_PROVIDERS.md
