<!-- SPDX-License-Identifier: GPL-3.0-or-later -->

# Infiltratr Common usage ledger

This ledger records why public Common operations exist and which production
consumers justify them. Tests alone do not justify new public API.

Status meanings: **ACTIVE** is used by production code, **FOUNDATION** is the
canonical implementation beneath active API, and **READY** is a tested
completion of an already-active capability family.

Audit baseline: Infiltratr Common 1.11.0, Calendar Plus 3.9.9, Linux System
Monitor 1.13.17 migration, Linux Defragger 1.8.x, LINK 0.9.1, MBLINK 0.7.25,
JAGLINK 0.2.12 and InfiltratorFS 0.9.x, 23 August 2026.

## Portable core

| Capability | Production consumers | Status |
| --- | --- | --- |
| Project identity validation and metadata output | System Monitor, Calendar Plus, LINK, InfiltratorFS | ACTIVE |
| Bounded copy, trimming and deterministic string comparisons | System Monitor, Calendar Plus, Linux Defragger, LINK | ACTIVE |
| Strict integer parsing and bounded unsigned parsing | System Monitor, Linux Defragger, LINK | ACTIVE |
| Locale-independent finite decimal parsing | Calendar Plus, POSIX typed readers | ACTIVE |
| Allocation-free key=value and boolean configuration parsing | Linux System Monitor | ACTIVE |
| Numeric clamping | System Monitor, Calendar Plus | ACTIVE |
| Checked/saturating unsigned arithmetic | System Monitor, Linux Defragger, LINK, InfiltratorFS | ACTIVE |
| Fixed-width little-endian conversion and byte swapping | InfiltratorFS, Linux Defragger | ACTIVE |
| Strict allocation-free UTF-8 validation | InfiltratorFS | ACTIVE |
| `infiltratr_i64_floor_divmod` | Calendar Plus date/time arithmetic | ACTIVE |
| `infiltratr_i64_subtract_saturating` | Calendar Plus event timing | ACTIVE |
| Percentages and rollback-safe counter rates | System Monitor | ACTIVE |
| Monotonic interval-due scheduling policy | Linux System Monitor | ACTIVE |
| Continuous period phase / upward millisecond conversion | Calendar Plus | ACTIVE |
| Exact signed-integer period remaining | Calendar Plus | ACTIVE |
| Exact rational cycle partition index / next-boundary distance | Calendar Plus | ACTIVE |
| Exact microsecond-to-millisecond upward conversion | Calendar Plus | ACTIVE |
| Missed-deadline cadence advancement | LINK | ACTIVE |
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
implementation.

Little-endian conversion and strict UTF-8 validation entered Common because
InfiltratorFS requires exactly those portable contracts in its on-disk format,
while byte-order conversion also duplicates code already present in Linux
Defragger. Filesystem-specific name policy remains in InfiltratorFS; Common owns
only encoding validity and fixed-width byte order.

Timing is split by mathematical domain rather than by application. Continuous
quantities retain the long-double period operation. Integral clocks use exact
Euclidean period arithmetic. Repeating integer cycles split into arbitrary
rational partitions without forming an overflowing `position * count` product;
this is the canonical implementation used by Calendar Plus for decimal,
Internet, hexadecimal and other discrete clock boundaries. The returned delay
is rounded upward in integer source units before exact microsecond-to-millisecond
conversion, so a timer cannot be armed before the displayed partition changes.
LINK's missed-deadline operation remains the canonical drift-free deadline
advancer used by the automotive layer, and System Monitor uses the elapsed
interval policy.

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
formatters currently used by Linux System Monitor. InfiltratorFS may use the
same base-2 byte formatter for user-facing capacity/status text, while on-disk
sizes and filesystem policy remain application-owned.

## POSIX provider

System Monitor and Linux Defragger use the POSIX path, file-reading and
monotonic-clock adapters. InfiltratorFS also uses Common's exact positioned
`pread`/`pwrite` loop contract while retaining block-device discovery, locking,
filesystem status translation, size queries, randomness and durability policy
inside its own POSIX storage adapter. Calendar Plus and LINK consume the
portable Common target for their portable cores rather than the POSIX provider.

Rich `*_ex` readers preserve missing, denied, empty, truncated, invalid-value
and generic I/O states. Simpler compatibility readers remain for consumers
whose existing contracts intentionally collapse those states. Exact positioned
I/O retries `EINTR`, rejects unrepresentable offsets and fails closed on
premature EOF or a zero-progress write.

## Compiler annotations

`INFILTRATR_LIKELY`, `INFILTRATR_UNLIKELY`, `INFILTRATR_COLD` and
`INFILTRATR_PRINTF_FORMAT` are active through Linux System Monitor's
compatibility facade.

## Consumer boundaries

- Linux System Monitor uses Common for general C primitives, configuration parsing, timing policy, formatting, dynamic loading and the POSIX provider while hardware/UI policy stays application-owned.
- Calendar Plus uses Common for generic parsing/string/arithmetic, exact discrete and continuous phase-aligned timing, and runtime library loading; calendar systems, astronomy, event indexing, ICU symbol lists/GVariant integration and Cinnamon UI remain application-owned.
- Linux Defragger uses Common where generic parsing/arithmetic/POSIX and byte-order contracts match, while raw filesystem safety and transaction semantics remain application-owned.
- LINK uses Common's portable target for general C primitives and timing policy while ELM327 framing, OBD/UDS/ISO-TP semantics, request policy, diagnostic sequencing and shared vehicle-diagnostics behaviour remain LINK-owned.
- MBLINK and JAGLINK consume Common transitively through their pinned LINK dependency; product repositories retain branding, metadata and genuinely manufacturer-specific behaviour rather than duplicating Common or LINK algorithms.
- InfiltratorFS uses Common for endian conversion, UTF-8 validity, checked arithmetic, shared identity/formatting where applicable and exact POSIX positioned I/O. Checkpoint recovery, allocation, CoW, on-disk ownership, Win32 raw-volume access and filesystem policy remain InfiltratorFS-owned.

## Build-package contract

Common owns the complete membership and dependency graph of its consumer build
targets. CMake consumers link `InfiltratrCommon::Portable` or
`InfiltratrCommon::Common`; Apple consumers link the
`InfiltratrCommonPortable` product from Common's Xcode subproject. Applications
must not copy an internal `.c` source list into their own build definitions.

This contract is ACTIVE through LINK, Linux System Monitor's 1.13.17 migration
and InfiltratorFS. LINK links `InfiltratrCommon::Portable` and then exposes its
own shared automotive target to MBLINK and JAGLINK. This prevents integration
omissions when Common's internal source/dependency graph changes.

## Rule for adding public API

A new public operation must satisfy at least one of these conditions:

1. two consumers already contain substantially the same capability;
2. a consumer needs a capability another consumer already has;
3. it completes an already-active shared capability robustly; or
4. application-private production code can be replaced immediately.

Once Common accepts an algorithmic responsibility, the implementation must
cover the complete declared input domain and define its boundary/failure
semantics. A consumer-specific special case is not a substitute for completing
the shared abstraction. Speculative capabilities with no production use remain
out of Common.