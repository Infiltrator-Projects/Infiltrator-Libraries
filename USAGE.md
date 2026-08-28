<!-- SPDX-License-Identifier: GPL-3.0-or-later -->

# Infiltratr Common usage ledger

This ledger records why public Common operations exist and where ownership belongs. Tests alone do not justify public API.

Status meanings: **ACTIVE** is consumed by production code, **FOUNDATION** is the canonical implementation beneath active API, and **READY** completes an already-active capability or directly replaces production-private mechanics when the consumer next advances its Common pin.

## Portable core

| Capability | Principal consumers / reason | Status |
| --- | --- | --- |
| Project identity and metadata | System Monitor, Calendar Plus, LINK, InfiltratorFS | ACTIVE |
| Bounded strings and trimming | System Monitor, Calendar Plus, Defragger, LINK | ACTIVE |
| Strict signed/unsigned whole-value parsing and ranges | System Monitor, Defragger, LINK | ACTIVE |
| Unsigned cursor token parsing | System Monitor procfs/sysfs parsing | ACTIVE |
| Signed cursor token parsing | Completes the active cursor-parser family | READY |
| Locale-independent finite decimal parsing | Calendar Plus, POSIX typed readers | ACTIVE |
| Strict binary quantity parsing | Replaces repeated K/M/G-style quantity parsing such as System Monitor cache-size input | READY |
| Checked and saturating arithmetic | System Monitor, Defragger, LINK, InfiltratorFS | ACTIVE |
| Checked unsigned subtract/multiply | Completes active checked-u64 family | READY |
| Checked `size_t` arithmetic / array reservation | System Monitor | ACTIVE |
| Little-endian conversion | InfiltratorFS, Defragger | ACTIVE |
| Big-endian conversion and byte load/store | Replaces repeated filesystem BE16/BE32 decoding in Defragger; completes endian family | READY |
| Strict UTF-8 validation | InfiltratorFS | ACTIVE |
| Exact timing / periodic cadence | Calendar Plus, System Monitor, LINK | ACTIVE |
| Generic quantity scaling | Formatting foundation | FOUNDATION |
| Shared metric/duration formatting | System Monitor | ACTIVE |
| Allocation-free i18n lookup/interpolation | LINK family | ACTIVE |

## ABI contract

Public ABI-bearing structures use `struct_size` and `abi_version`. The implementation validates the minimum prefix required by the declared ABI version rather than the newest compiler `sizeof(struct)`. This prevents harmless trailing padding or future appended fields from breaking older ABI-compatible callers while still rejecting incomplete structures.

## Dynamic-library adapter

`dynlib.c` owns portable module lifetime and symbol lookup over POSIX `dlopen`/`dlsym`/`dlclose` and Win32 `LoadLibrary`/`GetProcAddress`/`FreeLibrary`. Library names, version probing and required-symbol policy remain application-owned. Windows is now compiled and exercised directly in Common CI.

## POSIX provider

Common owns generic file/path/clock mechanics used by Linux applications:

- detailed text/u64/i64/double file readers;
- readable-path selection;
- durable atomic replacement;
- monotonic nanosecond/second clocks;
- exact EINTR-safe sequential descriptor reads/writes;
- exact EINTR-safe positioned pread/pwrite.

The signed i64 reader is a READY completion of the existing typed-reader family. Filesystem status translation, block-device discovery, locking, randomness and durability policy specific to a storage engine remain application-owned.

## Endian ownership

Common owns fixed-width byte-order mechanics, not filesystem formats. The BE/LE load/store helpers intentionally operate on byte storage without alignment assumptions, allowing Defragger and filesystem code to remove private `be16`, `be32`, `le16`, and similar decoder variants while preserving filesystem-specific structural validation in those projects.

## Localisation ownership

Common owns catalogue lookup, locale normalisation, language/fallback selection and named placeholder interpolation. Product language strings, language-pack storage/discovery, UI direction policy and manufacturer-specific presentation remain in the product or LINK layer.

Locale normalisation is deterministic ASCII logic. Malformed tags, duplicate separators and overlong tags fail instead of being silently shortened. POSIX `_` separators and `.encoding`/`@modifier` suffixes are accepted at the platform boundary.

## Build-package contract

CMake consumers link one authoritative target rather than enumerating Common source files:

- `InfiltratrCommon::Portable`
- `InfiltratrCommon::Common`
- `InfiltratrCommon::Shared`

The Windows build separates the static-library output from the DLL import library and exports the shared C API. Apple owns the complete portable source set through its Xcode project. CI verifies GCC, Clang, sanitizers, Windows MSVC and Apple Debug/Release.

## Consumer boundaries

- System Monitor: Common owns general C primitives, formatting, timing, dynamic loading and POSIX mechanics; hardware/UI policy remains System Monitor-owned.
- Calendar Plus: Common owns generic strings/parsing/arithmetic/timing/dynamic loading; chronology, astronomy and calendar/event semantics remain Calendar-owned.
- Defragger: Common owns general arithmetic, byte order and exact I/O; filesystem safety, on-disk validation and relocation transactions remain Defragger-owned.
- InfiltratorFS: Common owns endian/UTF-8/checked arithmetic/exact POSIX I/O; allocation, CoW, checkpoints, recovery and filesystem semantics remain InfiltratorFS-owned.
- LINK: Common owns portable primitives/localisation engine/timing; OBD/UDS/ISO-TP and vehicle-diagnostic policy remain LINK-owned.
- MBLINK/JAGLINK consume Common transitively through LINK where appropriate.

## Public API permanence

Once an operation is published as part of Common's public API, it remains part
of the reference library. Loss of a current production caller is not grounds
for removal: published operations may represent historical requirements and
are maintained as stable reference implementations. Existing public operations
must therefore continue to receive correctness fixes, complete contracts and
regression coverage even when their current consumer count is zero.

This permanence rule applies to accepted public API. It does not relax the
admission standard for new operations.

## Rule for adding public API

A new public operation must satisfy at least one of these conditions:

1. two consumers already contain substantially the same capability;
2. one consumer needs a capability another already has;
3. it robustly completes an already-active shared capability family; or
4. application-private production code can be replaced immediately.

Once Common accepts an algorithmic responsibility, it must define complete input, boundary and failure semantics. Speculative utility APIs with no production justification stay out of Common.
