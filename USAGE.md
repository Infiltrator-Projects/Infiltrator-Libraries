<!-- SPDX-License-Identifier: GPL-3.0-or-later -->

# Infiltratr Common usage ledger

This ledger records why public Common operations exist and which production
consumers justify them. Tests alone do not justify new public API.

Status meanings: **ACTIVE** is used by production code, **FOUNDATION** is the
canonical implementation beneath active API, and **READY** is a tested
completion of an already-active capability family.

Audit baseline: Infiltratr Common 1.8.0 migration, Calendar Plus 3.9.8,
Linux System Monitor 1.13.14, Linux Defragger 1.8.x and MBLINK 0.2.x,
20 August 2026.

## Portable core

| Capability | Production consumers | Status |
| --- | --- | --- |
| Project identity validation and metadata output | System Monitor, Calendar Plus, MBLINK | ACTIVE |
| Bounded copy, trimming and deterministic string comparisons | System Monitor, Calendar Plus, Linux Defragger, MBLINK | ACTIVE |
| Strict integer parsing and bounded unsigned parsing | System Monitor, Linux Defragger, MBLINK | ACTIVE |
| Locale-independent finite decimal parsing | Calendar Plus, POSIX typed readers | ACTIVE |
| Allocation-free key=value and boolean configuration parsing | Linux System Monitor | ACTIVE |
| Numeric clamping | System Monitor, Calendar Plus | ACTIVE |
| Checked/saturating unsigned arithmetic | System Monitor, Linux Defragger, MBLINK | ACTIVE |
| `infiltratr_i64_floor_divmod` | Calendar Plus date/time arithmetic | ACTIVE |
| `infiltratr_i64_subtract_saturating` | Calendar Plus event timing | ACTIVE |
| Percentages and rollback-safe counter rates | System Monitor | ACTIVE |
| Monotonic interval-due scheduling policy | Linux System Monitor | ACTIVE |
| Period phase / upward millisecond conversion | Calendar Plus | ACTIVE |
| Missed-deadline cadence advancement | MBLINK | ACTIVE |
| Quantity scaling and scaled rendering | Shared formatting implementation | FOUNDATION |

Signed floor division is shared because Calendar previously maintained the same
negative-safe quotient/remainder algorithm in its Julian-day, astronomical-time
and time-format modules. Saturating signed subtraction replaces Calendar's
private event-timing overflow helper. Calendar-specific chronology, calendar
providers and event semantics remain in Calendar Plus.

The signed parser/range variants not yet called directly by an application are
retained as READY members of the already-active strict parser family.

The configuration parser entered Common because Linux System Monitor immediately
replaced application-private key/value and boolean parsing with the shared
implementation. Timing now covers three distinct but related production needs:
System Monitor's elapsed refresh cadence, Calendar Plus's phase-aligned one-shot
clock boundaries, and MBLINK's drift-free advancement past missed periodic
requests.

## Dynamic-library adapter

`dynlib.c` centralises the native loader mechanics shared by Calendar Plus and
Linux System Monitor. It wraps POSIX `dlopen`/`dlsym`/`dlclose` and Win32
`LoadLibrary`/`GetProcAddress`/`FreeLibrary` without exposing either platform's
types to application code. Calendar retains ICU version probing and required
symbol policy; System Monitor retains NVML-specific discovery and metrics
policy. Common owns only module lifetime and safe symbol-pointer transfer.

## Formatting

Common owns the generic scalar and quantity-formatting engines plus the memory,
disk, network, percentage, frequency, temperature and power convenience
formatters currently used by Linux System Monitor. Calendar Plus does not use
shared presentation formatting and therefore does not compile `format.c` in its
application-local Common footprint.

## POSIX provider

System Monitor and Linux Defragger use the POSIX path, file-reading and
monotonic-clock adapters. Calendar Plus and MBLINK do not compile the POSIX
provider in their portable-core builds.

Rich `*_ex` readers preserve missing, denied, empty, truncated, invalid-value
and generic I/O states. Simpler compatibility readers remain for consumers
whose existing contracts intentionally collapse those states.

## Compiler annotations

`INFILTRATR_LIKELY`, `INFILTRATR_UNLIKELY`, `INFILTRATR_COLD` and
`INFILTRATR_PRINTF_FORMAT` are active through Linux System Monitor's
compatibility facade.

## Consumer boundaries

- Linux System Monitor uses Common for general C primitives, configuration parsing, timing policy, formatting, dynamic loading and the POSIX provider while hardware/UI policy stays application-owned.
- Calendar Plus uses Common for generic parsing/string/arithmetic, phase-aligned timing and runtime library loading; calendar systems, astronomy, event indexing, ICU symbol lists/GVariant integration and Cinnamon UI remain application-owned.
- Linux Defragger uses Common where generic parsing/arithmetic/POSIX contracts match, while raw I/O, filesystem safety and transaction semantics remain application-owned.
- MBLINK uses Common portable primitives and periodic deadline advancement while ELM327 framing, OBD/ISO-TP semantics, request policy and vehicle diagnostics remain application-owned.

## Rule for adding public API

A new public operation must satisfy at least one of these conditions:

1. two consumers already contain substantially the same capability;
2. a consumer needs a capability another consumer already has;
3. it completes an already-active shared capability robustly; or
4. application-private production code can be replaced immediately.

Every public API change updates this ledger. Speculative utility code stays out.
