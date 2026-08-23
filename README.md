<!-- SPDX-License-Identifier: GPL-3.0-or-later -->

# Infiltrator Libraries

[![Common CI](https://github.com/The-First-Infiltrator/Infiltrator-Libraries/actions/workflows/ci.yml/badge.svg)](https://github.com/The-First-Infiltrator/Infiltrator-Libraries/actions/workflows/ci.yml)

Infiltrator Libraries is the canonical shared-code repository for reusable first-party components used across the Infiltrator software family.

**Current library:** Infiltratr Common 1.11.0  
**Language:** C11  
**Licence:** GPL-3.0-or-later

## Role in the project family

```text
Infiltratr Common
      ├─ Calendar Plus
      ├─ System Monitor
      ├─ Defragger
      ├─ InfiltratorFS
      └─ LINK
           ├─ MBLINK
           └─ JAGLINK
```

Common owns portable facilities useful across multiple products. Application behaviour, filesystem semantics, hardware collectors, calendar rules, vehicle diagnostics and user interfaces remain in their owning repositories.

Once Common owns a general algorithmic contract, consumers provide policy rather than reimplementing smaller private variants. Speculative utility APIs stay out of Common until there is a real consumer requirement.

## Capabilities

The public API currently covers:

- project/build identity and bounded strings;
- strict integer, decimal, boolean and `key=value` parsing;
- checked/saturating arithmetic and Euclidean division;
- fixed-width endian conversion and strict allocation-free UTF-8 validation;
- exact elapsed/periodic timing and delay conversion;
- configurable scalar, byte, rate, percentage, frequency, temperature and power formatting;
- POSIX/Win32 dynamic-library lifetime and symbol lookup;
- path handling and bounded text/numeric file reads;
- exact EINTR-safe positioned POSIX I/O; and
- monotonic timing through the POSIX provider.

The portable core remains independent of GLib, GTK and OS APIs. POSIX and dynamic-loader adapters are isolated from the dependency-free modules.

## Architecture

| Path | Purpose |
| --- | --- |
| `include/infiltratr/` | Public headers and API contracts. |
| `src/core.c` | Parsing, project metadata, scaling and general primitives. |
| `src/arithmetic.c` | Checked, saturating and Euclidean arithmetic. |
| `src/config.c` | Portable configuration parsing. |
| `src/timing.c` | Exact elapsed/periodic timing policy. |
| `src/format.c` | Shared formatting. |
| `src/dynlib.c` | POSIX/Win32 dynamic-library adapter. |
| `src/posix.c` | POSIX path/file/clock provider. |
| `src/posix_io.c` | Exact positioned POSIX I/O. |
| `apple/InfiltratrCommon.xcodeproj` | Apple portable static-library target. |
| `USAGE.md` | Public API consumer/status ledger. |

CMake consumers link `InfiltratrCommon::Portable` or `InfiltratrCommon::Common`; consumers must not enumerate Common's internal source files themselves.

## Build and test

```sh
make check
make portable-check
make shared
cmake -S . -B cmake-build -DINFILTRATR_COMMON_BUILD_TESTS=ON
cmake --build cmake-build
ctest --test-dir cmake-build --output-on-failure
```

The default build creates `build/libinfiltratr-common.a`; the portable target creates `build/libinfiltratr-portable.a`; the shared target creates the versioned shared library with the stable major SONAME.

GitHub Actions runs strict GCC and Clang builds, portable-only verification, shared-library linking, CMake package/consumer checks, the Apple static-library target and sanitizer coverage.

## Release assets

A numbered Common release publishes:

| File | Purpose |
| --- | --- |
| `Infiltratr-Common-<version>-source.zip` | Exact tested source archive. |
| `SHA256SUMS.txt` | SHA-256 checksum for the source archive. |

Consumer repositories pin an exact reviewed Common release/commit rather than maintaining private forks.

## Repository and release policy

This repository uses `main` as its working branch. Development changes are made directly on `main`; the normal project workflow does not depend on PR, feature or release branches.

Every push to `main` runs Common CI. Ordinary commits do not publish. A commit is release-eligible only when its subject begins with the exact source version as `Release <version>` and the complete Common CI run succeeds.

The publisher checks out the exact tested commit, verifies it is still current `main`, reruns the Common build/test gates, creates the tested source ZIP and checksum, then creates the immutable version tag and GitHub release. Existing version tags and published releases are never moved, replaced or edited in place.

Manually runnable build/test helpers, where present, are diagnostic tools only and are not release-approval mechanisms.

## Source of truth

This repository is the authoritative copy of Infiltratr Common. Application source releases that vendor Common must match the pinned release source. LINK owns the shared automotive layer above Common; MBLINK and JAGLINK consume LINK instead of duplicating Common- or LINK-owned algorithms.

See `USAGE.md` for the public API consumer/status map.

## Licence

Copyright © 2026 Shannon Smith.

Infiltratr Common is free software licensed under the GNU General Public License version 3 or, at your option, any later version (`GPL-3.0-or-later`). The complete licence text is in `LICENSE`.
