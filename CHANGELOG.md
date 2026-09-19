# Changelog

This file records user-visible and contract-relevant changes to Common. Detailed commit history remains in Git; this document is for changes that matter to consumers.

## Unreleased

- Documentation baseline standardised across the Infiltrator project family.
- Continue to record ABI/API additions, behaviour changes, portability changes and fixes that affect consumers here.

## 1.19.7 — 2026-09-19

- Added exact locale-independent finite-decimal cursor-token parsing with unchanged-output failure semantics.
- Exposed canonical native structural design metrics and typography identity through the C adapter.
- Centralised immutable MB Corpo font-asset provenance in reusable CMake metadata without moving font binaries into Common.

## Release history

The repository already records release-specific detail in README.md, release tags and GitHub Releases. Those immutable release records remain authoritative for exact historical source identity.

## Changelog policy

Entries should describe behaviour, not internal activity. Refactoring with no observable contract change needs an entry only when it materially changes maintenance, portability or dependency requirements.
