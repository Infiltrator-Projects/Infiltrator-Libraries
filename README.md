<!-- SPDX-License-Identifier: GPL-3.0-or-later -->

# Infiltratr Libraries

Canonical shared C code for Shannon Smith's Infiltrator software projects.

The current release contains **Infiltratr Common 1.11.0**, the reusable C11
foundation consumed directly by Linux System Monitor, Calendar Plus, Linux
Defragger, LINK and InfiltratorFS. LINK then provides the shared automotive
engine used by the MBLINK and JAGLINK product faces. Application code, calendar
rules, hardware collectors, filesystem semantics, vehicle/manufacturer-specific
diagnostics and user-interface code remain in their owning repositories.

## Shared-code design rule

Common contains reusable implementations backed by real consumer requirements.
Application-specific behaviour stays with the application. Once Common owns an
algorithmic problem, it owns the complete general contract for that problem,
including boundary and failure semantics; consumers provide policy rather than
reimplementing smaller variants. Speculative utility APIs remain out of Common.

The consumer/status map in [USAGE.md](USAGE.md) records why each public
operation exists.

## Current library

The public API covers:

- project identity and machine-readable build metadata;
- bounded strings and deterministic string comparisons;
- strict signed/unsigned integer, range-checked and locale-independent ASCII-decimal parsing;
- allocation-free `key=value` and conservative boolean configuration parsing;
- checked and saturating arithmetic, signed floor division with Euclidean remainder, percentages and counter rates;
- fixed-width little-endian conversion and byte swapping;
- strict allocation-free UTF-8 validation;
- portable elapsed-time, exact integer periodic boundaries, exact rational cycle partitioning, continuous periodic-boundary and missed-deadline timing policy;
- exact upward microsecond-to-millisecond delay conversion;
- configurable quantity scaling with selectable divisor, unit range and precision;
- base-2 byte and byte-rate formatting;
- configurable scalar formatting plus shared memory, disk, network, percentage, frequency, temperature and power formatting;
- cross-platform dynamic-library lifetime and symbol-resolution mechanics for POSIX and Win32;
- path handling and small text/numeric file reads;
- exact EINTR-safe positioned POSIX descriptor reads and writes;
- detailed file-read status including missing, denied, empty, truncated and invalid values; and
- monotonic timing as either fractional seconds or exact integer nanoseconds through the POSIX provider.

`src/core.c`, `src/arithmetic.c`, `src/config.c`, `src/timing.c` and
`src/format.c` are independent of GLib, GTK and operating-system APIs. The
portable build contains those five modules plus header-only endian and UTF-8
contracts. `src/dynlib.c` is a thin POSIX/Win32 runtime-loader adapter;
`src/posix.c` is the Linux/POSIX file, path and monotonic-clock provider, and
`src/posix_io.c` owns exact positioned descriptor I/O.

## Layout

| Path | Purpose |
| --- | --- |
| `include/infiltratr/` | Public C headers and API contracts |
| `src/core.c` | Portable project, parsing, scaling and general numeric primitives |
| `src/arithmetic.c` | Portable signed Euclidean and saturating arithmetic |
| `src/config.c` | Portable key=value and boolean configuration parsing |
| `src/timing.c` | Portable elapsed, exact discrete-cycle and continuous periodic timing policy |
| `src/format.c` | Portable shared formatting implementation |
| `src/dynlib.c` | POSIX/Win32 dynamic-library adapter |
| `src/posix.c` | POSIX path, text-file and monotonic-clock implementation |
| `src/posix_io.c` | Exact positioned POSIX descriptor I/O |
| `CMakeLists.txt` | Authoritative CMake portable/full targets and installable package |
| `apple/InfiltratrCommon.xcodeproj` | Authoritative Apple portable static-library target |
| `USAGE.md` | Public API consumer/status ledger |
| `tests/` | Smoke, portable-contract, encoding, dynamic-loader, arithmetic and POSIX contract coverage |
| `.github/workflows/ci.yml` | GCC/Clang strict builds plus ASan/UBSan verification |
| `Makefile` | Static/shared-library build and tests |

## Build and test

```sh
make check
make portable-check
make shared
cmake -S . -B cmake-build -DINFILTRATR_COMMON_BUILD_TESTS=ON
cmake --build cmake-build
ctest --test-dir cmake-build --output-on-failure
```

`make check` runs all smoke and contract suites. `make portable-check` verifies
only the dependency-free modules and header-only portable contracts. The
default build creates `build/libinfiltratr-common.a`; `make portable` creates
`build/libinfiltratr-portable.a`. The shared target creates
`build/libinfiltratr-common.so.1.11.0` with SONAME
`libinfiltratr-common.so.1`.

CMake consumers that vendor Common use `add_subdirectory()` and link either
`InfiltratrCommon::Portable` or `InfiltratrCommon::Common`; consumers no longer
enumerate Common's internal source files. Installed packages expose the same
targets through `find_package(InfiltratrCommon 1.11 CONFIG REQUIRED)`.

Apple projects reference `apple/InfiltratrCommon.xcodeproj` and link its
`InfiltratrCommonPortable` product. That subproject owns the complete portable
source set, so adding an internal dependency to Common cannot silently leave an
application's Xcode target incomplete. Header-only endian and UTF-8 contracts
are available through the same public include directory without adding another
Apple source file.

GitHub Actions runs strict GCC and Clang builds, the portable-only target,
shared-library linking, CMake target/installation checks, the Apple static
library build and Clang AddressSanitizer/UndefinedBehaviorSanitizer.

Release publication is separate from CI. Ordinary pushes, successful CI runs
and tag pushes do not publish Common. The explicit `publish-release` workflow
checks out the exact current `main`, reruns the release gates, derives `vVERSION`
from that source tree, creates the immutable tag itself and refuses to modify an
existing tag or release.

## Source of truth

This repository is the authoritative copy of Infiltratr Common. Application
source releases that vendor Common must match the pinned release source and
must not become independent forks. LINK owns the shared vehicle-diagnostics
layer above Common; MBLINK and JAGLINK consume that layer rather than creating
parallel copies of Common-owned or LINK-owned algorithms.

## Licence

Copyright (C) 2026 Shannon Smith.

Infiltratr Common is free software licensed under the GNU General Public License,
version 3 or (at your option) any later version
(`GPL-3.0-or-later`). The complete licence text is in `LICENSE`.
