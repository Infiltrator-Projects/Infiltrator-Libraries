<!-- SPDX-License-Identifier: GPL-3.0-or-later -->

# Infiltratr Common usage ledger

This ledger records why each public Common operation exists and who currently consumes it. It is intended to keep the shared library focused on real reuse rather than speculative utility code.

Status meanings:

- **ACTIVE** — used by production code in at least one consumer.
- **FOUNDATION** — not necessarily called by an application directly, but is the canonical implementation underneath an ACTIVE shared operation.
- **READY** — a tested completion of an already-active capability family, but not currently required by production consumer code. Do not expand READY APIs further until a real consumer needs the extra behaviour.

Consumer notation:

- **System Monitor facade** means the operation is wired through the production `lsm_*` compatibility surface. Existing callers can keep the stable application API while Common owns the implementation.
- **Calendar Plus direct** means Calendar production source calls the Common API directly.
- **Linux Defragger native** means the filesystem-neutral C core or package-local native filesystem workers call the Common API directly.
- **Common internal** means another public Common operation uses it.
- Tests are not counted as production consumers.

Audit baseline: Infiltratr Common 1.5.0, Linux System Monitor 1.13.9, Calendar Plus 3.9.7 and Linux Defragger 1.8.0-100, 14 August 2026.

## Core

| Public operation | System Monitor | Calendar Plus | Linux Defragger | Common internal | Status | Reason |
| --- | --- | --- | --- | --- | --- | --- |
| `infiltratr_project_info_is_valid` | project-info validation/tests | — | — | `project_info_print` | FOUNDATION | Validates the shared project-identity model. |
| `infiltratr_project_info_print` | metadata/test tooling | direct (`--print-metadata`) | — | — | ACTIVE | One canonical machine-readable project metadata writer. |
| `infiltratr_copy_string` | facade | — | native journal workers | — | ACTIVE | Bounded copy replaces private fixed-buffer helpers. |
| `infiltratr_trim` | facade | — | — | — | ACTIVE | Shared in-place whitespace cleanup. |
| `infiltratr_trim_line_end` | facade | — | native journal workers | POSIX text reader | ACTIVE | Shared CR/LF cleanup and file-reader foundation. |
| `infiltratr_string_equal` | facade | direct | — | — | ACTIVE | Null-safe deterministic string equality used by consumers. |
| `infiltratr_string_starts_with` | facade | direct | native device core | — | ACTIVE | Shared prefix matching used across consumers. |
| `infiltratr_string_ends_with` | facade | — | — | — | ACTIVE | Shared suffix matching used by System Monitor. |
| `infiltratr_parse_u64` | facade | — | FAT/EXT/NTFS/exFAT/XFS journal workers | POSIX numeric readers | ACTIVE | Canonical strict unsigned parser. |
| `infiltratr_parse_i64` | — | — | — | — | READY | Completes the active integer parser family and is regression-tested. |
| `infiltratr_parse_u64_range` | — | — | FAT journals/workers, exFAT/AFFS/HFS+ native workers | — | ACTIVE | Canonical checked range validation used for filesystem-worker inputs and journal fields. |
| `infiltratr_parse_i64_range` | — | — | — | — | READY | Tested range validation for the signed parser family. |
| `infiltratr_parse_double` | — | direct | — | POSIX numeric readers | ACTIVE | Locale-independent machine-value parser used by Calendar Plus and Common's typed POSIX readers. |
| `infiltratr_parse_double_range` | — | — | — | — | READY | Tested bounded form of the active decimal parser. |
| `infiltratr_clamp_double` | facade | direct | — | — | ACTIVE | Shared numeric clamping used by both GUI consumers. |
| `infiltratr_u64_add_checked` | — | — | NTFS/XFS native arithmetic | — | ACTIVE | Canonical overflow-rejecting addition for filesystem range and committed-byte arithmetic. |
| `infiltratr_u64_add_saturating` | facade | — | EXT/NTFS/XFS staging estimates | — | ACTIVE | Canonical addition where capacity estimates intentionally saturate instead of wrapping. |
| `infiltratr_u64_multiply_saturating` | facade | — | EXT/NTFS/XFS staging/commit estimates | — | ACTIVE | Canonical multiplication where size estimates intentionally saturate instead of wrapping. |
| `infiltratr_percent_u64` | facade | — | — | — | ACTIVE | Canonical bounded percentage calculation. |
| `infiltratr_u64_counter_rate` | facade | — | — | — | ACTIVE | Canonical rollback-safe rate calculation. |
| `infiltratr_scale_quantity` | — | — | — | shared quantity/network formatters | FOUNDATION | One unit-selection algorithm for all scaled formatters. |
| `infiltratr_format_scaled_quantity` | — | — | — | byte/rate and metric wrappers | FOUNDATION | Canonical configurable scaled-quantity renderer. |
| `infiltratr_format_bytes` | facade | — | — | disk-capacity wrapper | ACTIVE | Shared binary byte presentation. |
| `infiltratr_format_rate` | facade | — | — | — | ACTIVE | Shared binary byte-rate presentation. |

## Formatting

| Public operation | System Monitor | Calendar Plus | Linux Defragger | Common internal | Status | Reason |
| --- | --- | --- | --- | --- | --- | --- |
| `infiltratr_format_scalar` | used through shared metric policy/adoption | — | — | percent/MHz/Celsius/watts wrappers | FOUNDATION | One configurable scalar renderer replaces separate formatting algorithms. |
| `infiltratr_format_memory_gb` | metric facade | — | — | — | ACTIVE | System Monitor memory presentation. |
| `infiltratr_format_disk_capacity` | metric facade | — | — | `format_bytes` | ACTIVE | System Monitor compact capacity presentation. |
| `infiltratr_format_network` | metric facade | — | — | scaling engine | ACTIVE | System Monitor network quantity/rate presentation. |
| `infiltratr_format_network_pair` | metric facade | — | — | scaling engine | ACTIVE | System Monitor shared-unit send/receive presentation. The hard-coded `S:`/`R:` policy should not be generalized further until another consumer needs different labels. |
| `infiltratr_format_link_speed_mbps` | metric facade | — | — | scaling engine | ACTIVE | System Monitor negotiated-link presentation. |
| `infiltratr_format_percent` | metric facade | — | — | scalar engine | ACTIVE | System Monitor optional percentage presentation. |
| `infiltratr_format_mhz` | metric facade | — | — | scalar engine | ACTIVE | System Monitor optional frequency presentation. |
| `infiltratr_format_celsius` | metric facade | — | — | scalar engine | ACTIVE | System Monitor optional temperature presentation. |
| `infiltratr_format_watts` | metric facade | — | — | scalar engine | ACTIVE | System Monitor optional power presentation. |

## POSIX provider

| Public operation | System Monitor | Calendar Plus | Linux Defragger | Common internal | Status | Reason |
| --- | --- | --- | --- | --- | --- | --- |
| `infiltratr_io_result_name` | — | — | — | — | READY | Human/machine-readable status naming for the active rich I/O family. |
| `infiltratr_realpath_copy` | facade | — | native device core | — | ACTIVE | Shared bounded `realpath` adapter. |
| `infiltratr_path_concat` | facade | — | — | `read_first_u64` | ACTIVE | Shared path construction for Linux collectors. |
| `infiltratr_path_join` | — | — | — | — | READY | Tested separator-aware form of the active path-helper family. |
| `infiltratr_read_text_file_ex` | — | — | — | rich typed readers | FOUNDATION | Preserves missing/denied/empty/truncated/error distinctions. |
| `infiltratr_read_u64_file_ex` | — | — | — | — | READY | Rich form of the active unsigned file reader. |
| `infiltratr_read_double_file_ex` | — | — | — | — | READY | Rich form of the active floating file reader. |
| `infiltratr_read_text_file` | facade | — | — | simple typed readers | ACTIVE | Fast small procfs/sysfs text reader. |
| `infiltratr_read_u64_file` | facade | — | native device core | `read_first_u64` | ACTIVE | Shared typed sysfs/procfs reader. |
| `infiltratr_read_u64_or_zero` | facade | — | — | — | ACTIVE | Convenience wrapper where zero and unavailable are intentionally equivalent. |
| `infiltratr_read_double_file` | facade | — | — | — | ACTIVE | Shared typed floating sysfs/procfs reader. |
| `infiltratr_read_double_or_nan` | facade | — | — | — | ACTIVE | Convenience wrapper preserving missing state as NaN. |
| `infiltratr_read_first_u64` | — | — | — | — | READY | Tested ordered fallback for alternate Linux attributes; retained as a completed POSIX capability until a production consumer needs it again. |
| `infiltratr_monotonic_nanoseconds` | — | — | — | — | READY | Tested exact form of the active monotonic-clock capability. |
| `infiltratr_monotonic_seconds` | facade | — | — | — | ACTIVE | System Monitor sampling clock. |

## Compiler annotations

`INFILTRATR_LIKELY`, `INFILTRATR_UNLIKELY`, `INFILTRATR_COLD` and `INFILTRATR_PRINTF_FORMAT` are ACTIVE through Linux System Monitor's `src/compiler.h` compatibility facade. Calendar Plus and Linux Defragger do not currently need them.

## Consumer build footprint

- Linux System Monitor 1.13.9 pins Infiltratr Common 1.5.0 and requires the portable core/formatting code plus the POSIX provider.
- Calendar Plus 3.9.7 pins Infiltratr Common 1.5.0 and consumes only the portable core/formatting code. It does not compile the Common POSIX provider because no production Calendar caller requires it.
- Linux Defragger 1.8.0-100 pins Common 1.5.0 at exact release commit `a0e75ffbe4e038c74c8f1e3d589f2dae87b2b7bb` and compiles Common `core.c` plus `posix.c`. It does not compile `format.c` because no production Defragger C caller currently needs shared presentation formatting.
- Linux Defragger now uses Common checked and saturating `uint64_t` arithmetic where those contracts match production filesystem work. It deliberately retains interruption-safe positional raw `pread`/`pwrite` loops, device safety, Stop handling and filesystem transaction semantics because those have different application-specific contracts.

## Rule for adding public API

Before a new public Common operation is added, it must satisfy at least one of these conditions:

1. two consumers already contain substantially the same capability;
2. a consumer needs a capability another consumer already has;
3. the operation is necessary to make an ACTIVE shared implementation robust or complete; or
4. application-private code can be replaced immediately by the new shared implementation.

Every public API change must update this ledger. A READY operation may remain because it completes an active capability family, but READY APIs should not accumulate additional speculative features until a production consumer needs them.
