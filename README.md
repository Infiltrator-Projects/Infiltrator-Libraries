<!-- SPDX-License-Identifier: GPL-3.0-or-later -->

# Infiltratr Libraries

Canonical shared C code for Shannon Smith's Infiltrator software projects.

The current release contains **Infiltratr Common 1.3.0**, the reusable C11
foundation shared by Calendar Plus and Linux System Monitor. Application code,
calendar rules, hardware collectors and user-interface code remain in their
own repositories.

## Shared-code design rule

A shared implementation should be the capable implementation, not merely the
lowest common denominator of the first applications that use it. Where one
consumer needs a simple form and another needs a richer form, Common should
provide the reusable capability and let each application select the behaviour
it needs. Convenience wrappers may expose simple common cases, but they should
sit on top of the canonical implementation rather than create parallel logic.

## Current library

The public API provides 43 reusable operations covering:

- project identity and machine-readable build metadata;
- bounded strings and deterministic string comparisons;
- strict unsigned-integer and locale-independent ASCII-decimal parsing;
- saturating arithmetic, percentages and counter rates;
- configurable quantity scaling with selectable divisor, unit range and precision;
- base-2 byte and byte-rate formatting;
- shared memory, disk, network, percentage, frequency, temperature and power formatting;
- path handling and small text/numeric file reads;
- detailed file-read status including missing, denied, empty, truncated and invalid values; and
- monotonic timing as either fractional seconds or exact integer nanoseconds through the POSIX provider.

`src/core.c` is independent of GLib, GTK and operating-system APIs.
`src/format.c` contains dependency-free presentation formatting shared by applications and uses the common scaling engine rather than maintaining separate unit-selection algorithms.
`src/posix.c` is the Linux/POSIX platform provider. Future Windows and UI
providers belong in this repository only when real shared implementations
exist.

The original simple APIs remain available for source compatibility. Newer
`*_ex` file APIs preserve failure details instead of forcing applications to
collapse every failure into a zero, `NAN`, or a generic false result.

## Layout

| Path | Purpose |
| --- | --- |
| `include/infiltratr/` | Public C headers |
| `src/core.c` | Portable implementation and canonical quantity scaling |
| `src/format.c` | Portable shared formatting implementation |
| `src/posix.c` | POSIX platform implementation |
| `tests/core_smoke.c` | Standalone core, status and scaling regression coverage |
| `tests/format_smoke.c` | Shared formatting regression coverage |
| `Makefile` | Static/shared-library build and tests |

## Build and test

```sh
make check
make shared
```

The default build creates `build/libinfiltratr-common.a`. The shared target
creates `build/libinfiltratr-common.so.1.3.0` with SONAME
`libinfiltratr-common.so.1`.

## Source of truth

This repository is the authoritative copy of Infiltratr Common. Copies used
to create self-contained application source releases must match the tagged
library source exactly and must not become independent forks.

## Licence

Copyright (C) 2026 Shannon Smith.

Infiltratr Common is free software licensed under the GNU General Public
License, version 3 or (at your option) any later version
(`GPL-3.0-or-later`). The complete licence text is in `LICENSE`.
