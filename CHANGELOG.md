# Changelog

This file records user-visible and contract-relevant changes to Common. Detailed commit history remains in Git; this document is for changes that matter to consumers.

## Unreleased

No unreleased changes.

## 1.19.27 — 2026-09-25

- Add a strict allocation-free Unicode scalar-to-UTF-8 encoder beside the existing validator.
- Reject UTF-16 surrogate code points and values above U+10FFFF while preserving exact one-through-four-byte canonical UTF-8 output.
- Keep filesystem, JSON and application-specific malformed-input/replacement policy in consumers; Common owns only the product-neutral scalar encoding step.
- Add contract coverage for ASCII, three-byte and maximum four-byte scalars plus invalid-scalar and insufficient-capacity unchanged-output failures.

## 1.19.26 — 2026-09-24

- Add one canonical elapsed-duration formatter keyed by the same system-wide clock-mode IDs used for civil-clock presentation.
- Give fixed-unit systems their actual elapsed representation: French decimal 10/100/100 time, Internet beats, binary H:M:S, hexadecimal 65,536-tick days, Julian/MJD fractional days, sidereal rate, Chinese double-hours/hundred-kè and Indian ghaṭī/vighaṭī.
- Anchor apparent-solar elapsed presentation to the interval end instant so equation-of-time drift is represented instead of silently treating apparent solar time as a fixed-rate clock.
- Keep equal-hour origin systems on their real 60/60 units, and deliberately retain conventional SI H:M:S for Roman and Edo seasonal period labels because their unequal day/night periods do not define one context-free duration unit.
- Preserve canonical SI/monotonic accounting; this API changes presentation only.

## 1.19.25 — 2026-09-24

- Add one portable formatter for every explicit clock mode in the shared temporal catalogue, including decimal, Internet, binary/hexadecimal, astronomical, solar-origin, Roman/Japanese seasonal and other historical systems.
- Keep the `standard` OS-locale mode deliberately platform-owned while making explicit 12/24-hour and extended modes deterministic across consumers.
- Require configured geographic context for location-dependent modes so applications cannot silently substitute Greenwich or another fabricated location.
- Add catalogue-wide regression coverage so System Settings cannot publish a selectable explicit mode that Common cannot render.

## 1.19.24 — 2026-09-22

- Harden the existing graphics rectangle/copy/blit paths so signed caller coordinates are clipped before endpoint arithmetic, eliminating overflow-prone `origin + length` calculations at extreme inputs.
- Make self-copy an explicit no-op success and snapshot aliased sources for in-place region copy, blitting, scaling and rotation so overlapping writes cannot destroy pixels that are still required as input.
- Reuse Common's existing exact cycle-partition arithmetic for nearest-neighbour scale mapping instead of forming a potentially overflowing `index * source_span` product.
- Add regression coverage for `INT_MIN`/`INT_MAX` clipping and overlapping in-place surface operations.
- No public functions, types or APIs were added or removed.

## 1.19.23 — 2026-09-22

- Expose Common's existing exact local-civil microsecond-of-day calculation as a public temporal primitive so Calendar and future consumers do not maintain a duplicate negative-safe day-phase algorithm.
- Preserve the formatter's existing behaviour by routing it through the promoted helper and add boundary coverage for negative instants, positive/negative UTC offsets and signed integer extremes.

## 1.19.22 — 2026-09-22

- Harden the existing bounded POSIX text reader to reject embedded NUL bytes in its retained byte range and trim trailing CR/LF directly from the known byte count instead of rediscovering length with `strlen()`.
- Reuse that bounded-reader contract for temporal provider and policy documents, removing duplicated open/read/probe/close loops while retaining the 1024-byte schema limit and existing rich I/O results.
- Anchor atomic file replacement and durable unlink to one opened parent-directory descriptor for target inspection, namespace mutation and final directory `fsync()`, closing the pathname re-resolution identity gap.
- Create atomic-writer temporary descriptors with `O_CLOEXEC` atomically on Linux via `mkostemp()`, retain the portable `mkstemp()+fcntl()` fallback elsewhere, and verify the temporary entry belongs to the anchored parent before publication.
- Add bounded embedded-NUL and symlink-target replacement regressions; the symlink test proves atomic replacement changes the link entry without modifying its victim.
- Leave `infiltratr_mkdir_parents()` unchanged; descriptor-relative traversal remains deferred until its existing generic pathname, symlink and permission semantics can be preserved exactly.
- No public functions, types or APIs were added or removed.

## 1.19.21 — 2026-09-22

- Harden the existing temporal-policy v3 parser so current authority documents must contain one unambiguous value for every existing v3 field; duplicate or incomplete v3 documents are rejected while legacy v1/v2 migration remains unchanged.
- Validate the existing fixed-size clock/calendar ID fields are NUL-terminated before serialisation performs catalogue lookups.
- Bound the existing POSIX temporal policy and provider-marker reads to the schema's 1024-byte document capacity while preserving embedded-NUL rejection and rich I/O outcomes.
- Accelerate the existing exact cycle-partition implementation with a checked direct-multiply path when the product fits in uint64_t, retaining the overflow-safe wide fallback unchanged.
- Strengthen tests for existing clock/calendar catalogue count/at/find invariants, bounded temporal files, embedded NULs and both cycle-partition arithmetic paths.
- No public functions, types or APIs were added or removed.

## 1.19.20 — 2026-09-21

- Validate the temporal provider marker's provider identity, policy version and contract instead of treating any readable file at the marker path as authority.
- Permit an explicit absolute provider-marker path for staged/custom-prefix installs and deterministic qualification.
- Reject non-finite and out-of-range latitude/longitude before serialising temporal policy, keeping save/load validation symmetric.
- Add allocation-backed XDG configuration-path resolution and remove the temporal persistence layer's private 4096-byte path ceiling.
- Extend POSIX and temporal contract tests for long XDG paths, provider marker contents and coordinate bounds.

## 1.19.19 — 2026-09-21

- Added the shared POSIX temporal-policy persistence adapter so Linux consumers no longer duplicate the XDG `presentation.conf` path, parsing, validation, directory creation or atomic write mechanics.
- Added the canonical installed temporal-provider capability marker `/usr/share/infiltrator/policy-providers/temporal-v3`, allowing consumers to distinguish an active System Settings authority from stale per-user policy files without probing executable names or PATH.
- Added contract tests for missing-policy defaults, canonical paths, atomic save/load round trips and provider-marker identity.

## 1.19.18 — 2026-09-20

- Removed the retired public temporal-policy v2 ABI instead of carrying compatibility baggage.
- Made temporal policy v3 the sole current cross-application authority.
- Removed the secondary-calendar entry and the non-calendar `none` choice from the shared calendar catalogue.
- Retained only a private one-time parser for already-written v2 `presentation.conf` data; the old primary calendar migrates to the single v3 calendar and the retired secondary value is discarded.
- Added regression coverage proving v3 serialisation cannot emit secondary-calendar state and the shared calendar catalogue contains only the 30 real calendar systems.

## 1.19.17 — 2026-09-20

- Added a generic monotonic unsigned-counter delta contract with unchanged-output rollback semantics and made the existing counter-rate operation consume it.
- Added errno-style POSIX absolute-deadline construction for caller-selected clocks and upward-rounded remaining-millisecond conversion.
- Kept pthread wait policy, hardware-counter interpretation and product-specific baseline/reset semantics in consumers while removing their repeated generic arithmetic.

## 1.19.16 — 2026-09-20

- Expanded temporal policy to a version-2 system authority containing all 21 Calendar clock systems.
- Added the complete shared calendar-system catalogue: 30 primary calendars plus None for the optional secondary date.
- Added primary calendar, secondary calendar, seconds and optional geographic latitude/longitude to the persisted temporal policy.
- Removed the conceptual global "Follow system" choice: the authority uses Standard time (OS locale), while applications may separately offer Follow System Settings.
- Added automatic in-memory migration of version-1 system/12h/24h/decimal policy documents to v2.

## 1.19.15 — 2026-09-20

- Completed the deterministic ASCII family with locale-independent case-insensitive lexical ordering, allowing consumers to remove residual `strcasecmp` sorting.
- Added stable non-cryptographic 64-bit FNV-1a byte, text and explicit little-endian uint64 mixing for consumer runtime identities/change signatures.
- Kept signature composition and domain identity policy in consumers; Common owns only the portable hash mechanics.

## 1.19.14 — 2026-09-20

- Added the portable system-wide temporal presentation policy contract with stable System, 12-hour, 24-hour and decimal-10 clock profiles.
- Added deterministic versioned policy parsing/serialization so System Settings and consumers can exchange one user-wide temporal preference without changing canonical data.
- Promoted Calendar's exact rational decimal-day partition semantics into Common and added boundary/extreme-instant regression coverage.

## 1.19.13 — 2026-09-20

- Added toolkit-neutral current-user HOME, XDG_CONFIG_HOME and XDG_DATA_HOME resolution to the POSIX provider, with specification-correct fallback for relative XDG overrides.
- Added recursive POSIX directory creation with errno-style failure reporting, replacing graphical-toolkit filesystem helpers in consumers without moving application policy into Common.
- Completed the public deterministic ASCII family with whitespace/alpha/digit/alnum/hex classification and case-insensitive substring matching, replacing locale/toolkit helpers in kernel, protocol and identifier grammars.
- Extended POSIX and core contract tests to cover environment overrides, XDG fallback behaviour, truncation, recursive directory creation and the complete ASCII contracts.

## 1.19.12 — 2026-09-20

- Promoted deterministic ASCII byte case conversion, complete case-insensitive equality and case-insensitive prefix matching into the portable public contract for protocol and hardware identifiers.
- Added a bounded POSIX lexical dirname contract alongside basename, allowing consumers to remove toolkit allocation from simple parent-directory extraction.

## 1.19.11 — 2026-09-20

- Added canonical lowercase System/Day/Night persistence keys and case-insensitive parsing to the shared design contract, replacing duplicated consumer serialization logic.
- Added the shared two-decimal GHz formatter used by System Monitor, completing the active frequency-formatting family without a consumer-local scalar wrapper.
- Standardised the documentation baseline across the Infiltrator project family.

## 1.19.7 — 2026-09-19

- Added exact locale-independent finite-decimal cursor-token parsing with unchanged-output failure semantics.
- Exposed canonical native structural design metrics and typography identity through the C adapter.
- Centralised immutable MB Corpo font-asset provenance in reusable CMake metadata without moving font binaries into Common.

## Release history

The repository already records release-specific detail in README.md, release tags and GitHub Releases. Those immutable release records remain authoritative for exact historical source identity.

## Changelog policy

Entries should describe behaviour, not internal activity. Refactoring with no observable contract change needs an entry only when it materially changes maintenance, portability or dependency requirements.
