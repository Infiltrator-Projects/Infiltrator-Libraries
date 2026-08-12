<!-- SPDX-License-Identifier: GPL-3.0-or-later -->

# Infiltratr Libraries

Canonical shared C code for Shannon Smith's Infiltrator software projects.

The current release contains **Infiltratr Common 1.1.1**, the reusable C11
foundation shared by Calendar Plus and Linux System Monitor. Application code,
calendar rules, hardware collectors and user-interface code remain in their
own repositories.

## Current library

The public API provides 27 reusable operations covering:

- project identity and machine-readable build metadata;
- bounded strings and deterministic string comparisons;
- strict unsigned-integer and locale-independent ASCII-decimal parsing;
- saturating arithmetic, percentages and counter rates;
- base-2 byte and byte-rate formatting;
- path handling, small text and numeric file reads; and
- monotonic timing through the POSIX provider.

`src/core.c` is independent of GLib, GTK and operating-system APIs.
`src/posix.c` is the Linux/POSIX platform provider. Future Windows and UI
providers belong in this repository only when real shared implementations
exist.

## Layout

| Path | Purpose |
| --- | --- |
| `include/infiltratr/` | Public C headers |
| `src/core.c` | Portable implementation |
| `src/posix.c` | POSIX platform implementation |
| `tests/core_smoke.c` | Standalone regression coverage |
| `Makefile` | Static/shared-library build and tests |

## Build and test

```sh
make check
make shared
```

The default build creates `build/libinfiltratr-common.a`. The shared target
creates `build/libinfiltratr-common.so.1.1.1` with SONAME
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
