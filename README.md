<!-- SPDX-License-Identifier: GPL-3.0-or-later -->

# Infiltrator Libraries

[![Common CI](https://github.com/Infiltrator-Projects/Infiltrator-Libraries/actions/workflows/ci.yml/badge.svg)](https://github.com/Infiltrator-Projects/Infiltrator-Libraries/actions/workflows/ci.yml)

Infiltrator Libraries is the canonical shared-code repository for reusable first-party components used across the Infiltrator software family.

**Current library version:** 1.15.6  
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

## Common 1.15.5

1.15.5 completes Common's signed 64-bit checked-arithmetic family for consumers
that accept wide integer domains. It adds checked signed addition and
multiplication plus saturating signed addition, with unchanged-output failure
semantics for checked operations. Calendar Plus can now harden extreme
chronology/navigation intermediates without carrying private overflow logic.

## Common 1.15.4

1.15.4 adds the durable namespace-removal counterpart to Common's existing
atomic replacement primitive. `infiltratr_unlink_durable()` resolves the parent
before unlinking, provides explicit missing-file semantics, fsyncs the parent
directory after a successful removal, and reports directory-sync failure without
pretending the already-issued unlink can be rolled back. Defragger can therefore
remove completed recovery journals through one tested durability contract rather
than duplicating unlink-plus-directory-fsync sequences across filesystem engines.

## Common 1.15.3

1.15.3 completes the post-1.15.2 reference-quality hardening pass without
changing the public ABI:

- published Common APIs are explicitly permanent once admitted; loss of a
  current caller is not grounds for removal;
- binary quantity parsing has no fixed token-length limit and remains exact
  across the uint64_t result domain;
- decimal-to-binary64 parsing is deterministically and correctly rounded,
  independent of host long-double precision, locale and active FP rounding mode;
- POSIX numeric/text readers remove fixed-size input/path limits and reject
  embedded-NUL/truncated success cases;
- formatting failure semantics are non-truncating, byte units cover the full
  uint64_t range through EB, and i18n length overflow saturates at SIZE_MAX;
- link-speed formatting now uses Common's intentional 1024-based Kb/Mb/Gb
  convention consistently instead of mixing decimal-million input conversion
  with binary scaling;
- positioned pread/pwrite requests prevalidate the entire off_t range before any
  I/O, preventing predictable partial transfers before EOVERFLOW;
- POSIX dynamic symbol lookup now distinguishes loader errors from a valid NULL
  symbol value, and library close state is cleared only after successful unload;
- endian conversion no longer silently assumes an unrecognised host is
  big-endian, using an explicit runtime fallback when compile-time byte order is
  unavailable;
- POSIX path-join semantics are documented and regression-tested as lexical
  boundary joining rather than implicit path normalization.

## Common 1.15.2

1.15.2 hardens Common's low-level contracts without changing the ABI:

- binary quantity parsing is exact across the full uint64_t domain and never
  routes integer quantities through floating point;
- bounded string copying is overlap-safe;
- exact POSIX descriptor I/O chunks oversized requests and validates positioned
  offsets against the native signed off_t width;
- Windows dynamic-library paths are validated as UTF-8 and loaded through the
  Unicode LoadLibraryW API;
- token-parser documentation now matches its established leading-whitespace
  behaviour, with regression coverage for all of these boundaries.

## Common 1.15.1

1.15.1 makes release publication atomic: the complete draft and its verified
source/checksum asset set are staged before the immutable tag becomes public.
It also incorporates the post-1.15.0 repository and release-policy
documentation into an exact, tagged Common dependency for downstream builds.

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
