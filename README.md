<!-- SPDX-License-Identifier: GPL-3.0-or-later -->

# Infiltratr Libraries

Canonical shared C code for Shannon Smith's Infiltrator software projects.

The current release contains **Infiltratr Common 1.7.0**, the reusable C11
foundation shared by Linux System Monitor, Calendar Plus, Linux Defragger and
MBLINK. Application code, calendar rules, hardware collectors, filesystem
semantics, vehicle-diagnostics logic and user-interface code remain in their
own repositories.

## Shared-code design rule

Common contains reusable implementations backed by real consumer requirements.
Application-specific behaviour stays with the application. Convenience wrappers
may expose common cases, but they sit on the canonical shared implementation
rather than duplicating it.

The consumer/status map in [USAGE.md](USAGE.md) records why each public
operation exists.

## Current library

The public API covers:

- project identity and machine-readable build metadata;
- bounded strings and deterministic string comparisons;
- strict signed/unsigned integer, range-checked and locale-independent ASCII-decimal parsing;
- allocation-free `key=value` and conservative boolean configuration parsing;
- checked and saturating arithmetic, signed floor division with Euclidean remainder, percentages and counter rates;
- portable interval-due policy for monotonic refresh loops;
- configurable quantity scaling with selectable divisor, unit range and precision;
- base-2 byte and byte-rate formatting;
- configurable scalar formatting plus shared memory, disk, network, percentage, frequency, temperature and power formatting;
- path handling and small text/numeric file reads;
- detailed file-read status including missing, denied, empty, truncated and invalid values; and
- monotonic timing as either fractional seconds or exact integer nanoseconds through the POSIX provider.

`src/core.c`, `src/arithmetic.c`, `src/config.c`, `src/timing.c` and
`src/format.c` are independent of GLib, GTK and operating-system APIs. The
`portable` build contains those five modules only. `src/posix.c` is the
Linux/POSIX provider.

## Layout

| Path | Purpose |
| --- | --- |
| `include/infiltratr/` | Public C headers and API contracts |
| `src/core.c` | Portable project, parsing, scaling and general numeric primitives |
| `src/arithmetic.c` | Portable signed Euclidean and saturating arithmetic |
| `src/config.c` | Portable key=value and boolean configuration parsing |
| `src/timing.c` | Portable monotonic-interval policy |
| `src/format.c` | Portable shared formatting implementation |
| `src/posix.c` | POSIX platform implementation |
| `USAGE.md` | Public API consumer/status ledger |
| `tests/` | Smoke, portable-contract, arithmetic and POSIX contract coverage |
| `.github/workflows/ci.yml` | GCC/Clang strict builds plus ASan/UBSan verification |
| `Makefile` | Static/shared-library build and tests |

## Build and test

```sh
make check
make portable-check
make shared
```

`make check` runs all smoke and contract suites. `make portable-check` verifies
only the dependency-free modules. The default build creates
`build/libinfiltratr-common.a`; `make portable` creates
`build/libinfiltratr-portable.a`. The shared target creates
`build/libinfiltratr-common.so.1.7.0` with SONAME
`libinfiltratr-common.so.1`.

GitHub Actions runs strict GCC and Clang builds, the portable-only target,
shared-library linking and Clang AddressSanitizer/UndefinedBehaviorSanitizer.

## Source of truth

This repository is the authoritative copy of Infiltratr Common. Application
source releases that vendor Common must match the pinned release source and
must not become independent forks.

## Licence

Copyright (C) 2026 Shannon Smith.

Infiltratr Common is free software licensed under the GNU General Public
License, version 3 or (at your option) any later version
(`GPL-3.0-or-later`). The complete licence text is in `LICENSE`.
