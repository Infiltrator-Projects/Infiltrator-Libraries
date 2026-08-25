<!-- SPDX-License-Identifier: GPL-3.0-or-later -->

# Infiltrator Libraries

[![Common CI](https://github.com/The-First-Infiltrator/Infiltrator-Libraries/actions/workflows/ci.yml/badge.svg)](https://github.com/The-First-Infiltrator/Infiltrator-Libraries/actions/workflows/ci.yml)

Infiltrator Libraries is the canonical shared-code repository for reusable first-party components used across the Infiltrator software family.

**Current library version:** 1.15.0  
**Language:** C11  
**Licence:** GPL-3.0-or-later

## Role

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

Common owns portable mechanics and algorithms that have real use across the project family. Application behaviour, filesystem semantics, hardware policy, calendar rules, vehicle diagnostics and user interfaces remain in their owning repositories.

## Common 1.15.0

1.15 completes several capability families that were already active in Common:

- checked unsigned add/subtract/multiply and saturating arithmetic;
- strict whole-value signed/unsigned parsing, range parsing, signed and unsigned cursor-token parsing;
- strict locale-independent finite decimal parsing;
- strict binary quantity parsing from B/K/KB/KiB through E/EB/EiB;
- little- and big-endian CPU conversion plus unaligned byte load/store helpers for 16/32/64-bit values;
- strict UTF-8 validation;
- exact EINTR-safe sequential `read`/`write` and positioned `pread`/`pwrite` contracts;
- rich POSIX text/u64/i64/double readers with explicit failure categories;
- durable atomic replacement writes and monotonic clocks;
- allocation-free localisation lookup/interpolation with strict non-truncating locale normalisation;
- quantity, percentage, frequency, temperature, power and duration formatting;
- POSIX/Win32 dynamic-library lifetime and symbol lookup;
- stable ABI-prefix validation for versioned option structures.

The portable core remains independent of GLib, GTK and OS APIs. POSIX and native loader adapters are isolated from dependency-free modules.

## ABI policy

The shared library retains major SONAME 1. ABI-bearing public structures begin with `struct_size` and `abi_version`. Version-1 implementations require the supplied structure to cover all fields defined by ABI 1, not `sizeof()` the newest compiler layout. Trailing padding and future appended fields therefore do not make an older ABI-1 caller invalid.

An incompatible semantic layout requires a new ABI version; an incompatible shared-library contract requires a new library major version.

## Build targets

CMake exports:

- `InfiltratrCommon::Portable` — dependency-free static portable core;
- `InfiltratrCommon::Common` — full static Common target;
- `InfiltratrCommon::Shared` — versioned shared Common target.

The Makefile continues to build `libinfiltratr-portable.a`, `libinfiltratr-common.a`, and the versioned `libinfiltratr-common.so.<version>` on POSIX systems.

```sh
make check
make portable-check
make shared
cmake -S . -B cmake-build -DINFILTRATR_COMMON_BUILD_TESTS=ON
cmake --build cmake-build
ctest --test-dir cmake-build --output-on-failure
```

## Verification

Every push to `main` runs:

- strict GCC and Clang builds;
- portable/full contract suites;
- CMake install and external-consumer tests;
- shared-library linkage tests;
- Clang ASan + UBSan;
- Windows MSVC static/shared CMake builds and tests;
- Apple Debug and Release portable-library builds.

## Repository and release policy

`main` is the working branch. Ordinary commits do not publish. A commit becomes release-eligible only when its subject begins with `Release <version>` and the complete Common CI run succeeds.

The publisher verifies the exact tested `main` commit, rebuilds the source package, creates the immutable version tag and GitHub release, and publishes the source archive plus SHA-256 checksums. Published tags/releases are never moved or edited in place.

## Source of truth

This repository is the authoritative copy of Infiltratr Common. Consumers pin an exact reviewed Common release/commit rather than maintaining private variants. See `USAGE.md` for capability ownership and consumer boundaries.

## Licence

Copyright © 2026 Shannon Smith.

Infiltratr Common is licensed under GNU GPL-3.0-or-later. See `LICENSE`.
