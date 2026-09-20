# Changelog

This file records user-visible and contract-relevant changes to Common. Detailed commit history remains in Git; this document is for changes that matter to consumers.

## Unreleased

No unreleased changes.

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
