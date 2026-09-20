# Changelog

This file records user-visible and contract-relevant changes to Common. Detailed commit history remains in Git; this document is for changes that matter to consumers.

## Unreleased

No unreleased changes.

## 1.19.13 — 2026-09-20

- Added toolkit-neutral current-user HOME, XDG_CONFIG_HOME and XDG_DATA_HOME resolution to the POSIX provider, with specification-correct fallback for relative XDG overrides.
- Added recursive POSIX directory creation with errno-style failure reporting, replacing graphical-toolkit filesystem helpers in consumers without moving application policy into Common.
- Extended POSIX contract tests to cover environment overrides, XDG fallback behaviour, truncation and recursive directory creation.

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
