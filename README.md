<!-- SPDX-License-Identifier: GPL-3.0-or-later -->

# Infiltratr Libraries

Canonical shared C code for Shannon Smith's Infiltrator software projects.

The current release contains **Infiltratr Common 1.5.0**, the reusable C11
foundation shared by Linux System Monitor, Calendar Plus, Linux Defragger and
MBLINK. Application code, calendar rules, hardware collectors, filesystem
semantics, vehicle-diagnostics logic and user-interface code remain in their
own repositories.

## Shared-code design rule

A shared implementation should be the capable implementation, not merely the
lowest common denominator of the first applications that use it. Where one
consumer needs a simple form and another needs a richer form, Common should
provide the reusable capability and let each application select the behaviour
it needs. Convenience wrappers may expose simple common cases, but they should
sit on top of the canonical implementation rather than create parallel logic.

Common is not a holding area for hypothetical helpers. New public API should
come from real consumer duplication, a real consumer requirement, or work
needed to make an already-shared capability robust and complete. The
consumer/status map in [USAGE.md](USAGE.md) records why every public operation
exists and must be updated when the public API or consumer usage changes.

## Current library

The public API provides 49 reusable operations covering:

- project identity and machine-readable build metadata;
- bounded strings and deterministic string comparisons;
- strict signed/unsigned integer, range-checked and locale-independent ASCII-decimal parsing;
- checked and saturating arithmetic, percentages and counter rates;
- configurable quantity scaling with selectable divisor, unit range and precision;
- base-2 byte and byte-rate formatting;
- configurable scalar formatting plus shared memory, disk, network, percentage, frequency, temperature and power formatting;
- path handling and small text/numeric file reads;
- detailed file-read status including missing, denied, empty, truncated and invalid values; and
- monotonic timing as either fractional seconds or exact integer nanoseconds through the POSIX provider.

`src/core.c` is independent of GLib, GTK and operating-system APIs. The
`portable` build contains only `core.c` and `format.c`, so consumers that do not
need POSIX services do not have to carry the POSIX provider. `src/format.c`
contains dependency-free presentation formatting shared by applications and
uses the common scaling engine rather than maintaining separate unit-selection
algorithms. `src/posix.c` is the Linux/POSIX platform provider. Future Windows
and UI providers belong in this repository only when real shared
implementations exist.

The original simple APIs remain available for source compatibility. Newer
`*_ex` file APIs preserve failure details instead of forcing applications to
collapse every failure into a zero, `NAN`, or a generic false result.

## Layout

| Path | Purpose |
| --- | --- |
| `include/infiltratr/` | Public C headers and API contracts |
| `src/core.c` | Portable implementation and canonical quantity scaling |
| `src/format.c` | Portable shared formatting implementation |
| `src/posix.c` | POSIX platform implementation |
| `USAGE.md` | Public API consumer/status ledger and anti-speculation rule |
| `tests/core_smoke.c` | Standalone core, status and scaling regression coverage |
| `tests/format_smoke.c` | Shared formatting regression coverage |
| `tests/portable_smoke.c` | POSIX-free smoke coverage |
| `tests/portable_contract.c` | POSIX-free boundary and failure-contract coverage |
| `tests/posix_contract.c` | POSIX path, I/O and monotonic-clock contract coverage |
| `.github/workflows/ci.yml` | GCC/Clang strict builds plus ASan/UBSan verification |
| `Makefile` | Static/shared-library build and tests |

## Build and test

```sh
make check
make portable-check
make shared
```

`make check` runs all smoke and contract suites. `make portable-check` builds
only `core.c` and `format.c` and runs both the portable smoke suite and the full
portable contract suite, ensuring the dependency-free library can be verified
without the POSIX provider. The default build creates
`build/libinfiltratr-common.a`; `make portable` creates the POSIX-free
`build/libinfiltratr-portable.a`. The shared target creates
`build/libinfiltratr-common.so.1.5.0` with SONAME
`libinfiltratr-common.so.1`.

GitHub Actions independently runs the strict test matrix with GCC and Clang,
verifies the portable-only target and shared-library link, and runs the complete
test suite under Clang AddressSanitizer and UndefinedBehaviorSanitizer.

## Source of truth

This repository is the authoritative copy of Infiltratr Common. Copies used
to create self-contained application source releases must match the tagged
library source exactly and must not become independent forks.

## Licence

Copyright (C) 2026 Shannon Smith.

Infiltratr Common is free software licensed under the GNU General Public
License, version 3 or (at your option) any later version
(`GPL-3.0-or-later`). The complete licence text is in `LICENSE`.