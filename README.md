<!-- SPDX-License-Identifier: GPL-3.0-or-later -->

# Common

**Project copyright:** © 1993-2026 Shannon Smith

[![Common CI](https://github.com/Infiltrator-Projects/Infiltrator-Libraries/actions/workflows/ci.yml/badge.svg)](https://github.com/Infiltrator-Projects/Infiltrator-Libraries/actions/workflows/ci.yml)

Common is the canonical shared-code repository for reusable first-party components used across the software family.

**Current library version:** 1.19.31
**Language:** C11  
**Licence:** GPL-3.0-or-later

## Engineering ethos

What happens when shared infrastructure is built from first principles and treated as an engineering product rather than a dumping ground for copied helpers? Common is where reusable contracts become first-party code: parsing, arithmetic, I/O, presentation primitives and other mechanics that have proved they belong to more than one project.

Common is deliberately conservative about ownership. A capability moves here only when the shared contract is at least as strong as the best implementation already present in a consumer. Project-specific semantics stay with their project, and an external dependency is not adopted merely to avoid writing code when owning the behaviour gives a clearer, more stable contract. Consumers pin exact reviewed Common revisions so a later upstream change cannot silently redefine their behaviour.

"Newer" is not automatically "better". Proven algorithms and interfaces remain when they are the stronger engineering choice; replacements need a measurable improvement in correctness, portability, resilience, performance or maintainability. The objective is one dependable shared foundation whose behaviour the project family controls and can explain.

## Role

```text
Common
      ├─ Calendar Plus
      ├─ System Monitor
      ├─ Defragger
      ├─ InfiltratorFS
      ├─ ssmithnet.net
      ├─ Infiltrator Repository
      └─ LINK
           ├─ MBLINK
           ├─ JAGLINK
           ├─ BMWLINK
           ├─ AUDILINK
           └─ FORDLINK
```

Common owns portable mechanics and algorithms that have real use across the project family. Product-neutral automotive diagnostic and shared application behaviour belongs in LINK; manufacturer-specific vehicle knowledge, branding and product-specific behaviour remain in MBLINK, JAGLINK, BMWLINK, AUDILINK and FORDLINK. Filesystem semantics, calendar rules and other domain-specific behaviour remain in their owning repositories rather than moving into Common merely because they are reusable within one product family.

Common also owns cross-project presentation/build primitives when they have the same neutral contract across products. The canonical graphical design tokens live under `design/`; a generated web adapter exposes those tokens to static web consumers without transferring product identity into Common. Generic HTML/JSON/URI output escaping belongs in the portable library, durable file publication belongs in the POSIX provider, and the reusable GitHub Pages artifact/deploy sequence belongs in a repository-local composite action. Page content, layout composition, application/domain semantics, status meanings, product accents, illustrations and release policy remain local to each consumer.

## Common 1.19.31

1.19.31 removes the remaining naming ambiguity in the historical clock catalogue. Modern Italian civil time is now an explicit 24-hour presentation choice, while the existing sunset-origin system is labelled historical. Ancient Babylonian time now uses the attested fixed bēru/UŠ system from a sunset-start civil day rather than the later seasonal-hour reconstruction. Nuremberg is split into the historical Nürnberg Great Clock, using the city's fixed Wendetag schedule, and a separately named location-aware solar reconstruction.

## Common 1.19.30

1.19.30 separates several historically overloaded clock names and corrects the Nuremberg model. The old `italian-hours` ID is now explicitly labelled as the historical sunset-origin 24-equal-hour system; modern Italian civil time remains ordinary standard civil time selected with the Europe/Rome time zone rather than a duplicate clock arithmetic. The existing sunrise-origin `babylonian-hours` mode is explicitly identified as the Renaissance European convention called Babylonian hours, while a new `babylonian-ancient` mode models the attested ancient seasonal scheme of twelve daylight and twelve night simānu. Nuremberg hours now use a discrete Wendetag-style integer allocation of equal hours between day and night, reconstructed from local solar day length, instead of incorrectly resetting at each actual sunrise and sunset.

## Common 1.19.29

1.19.29 makes historical elapsed-time presentation read in the selected system's own hierarchy. Roman seasonal durations now collapse each complete twelve-horae/four-vigiliae cycle into a `dies` before showing residual horae, vigiliae and unciae. Chinese hundred-kè durations now use native 日/刻 units instead of hybrid modern day prefixes and denominator notation; the generic hundred-kè mode deliberately does not invent a dynasty-specific sub-kè precision.

## Common 1.19.28

1.19.28 makes anchored elapsed durations culturally native for the two unequal seasonal clock families. Roman intervals now integrate the actual local daylight horae and night vigiliae across their changing astronomical boundaries, using unciae for fractional precision. Edo intervals integrate the six daytime and six nighttime seasonal koku/toki and use the historically attested half-period marker rather than modern minutes or seconds. Unanchored accumulated quantities remain explicitly labelled SI because a seasonal conversion without a civil interval would be fabricated.

## Common 1.19.27

1.19.27 centralises Unicode scalar-to-UTF-8 encoding beside Common's existing strict UTF-8 validator. The new allocation-free encoder rejects surrogate code points and values above U+10FFFF, reports exact one-to-four-byte output length and leaves caller storage unchanged on failure. Calculator and filesystem consumers can now retain their own Unicode-policy decisions while sharing one canonical byte encoder.

## Common 1.19.26

1.19.26 adds canonical elapsed-duration presentation for every system clock mode. Fixed-unit systems now expose their real duration units (decimal, Internet beats, binary, hexadecimal, Julian-day fractions, sidereal, Chinese day partitions and Indian ghaṭī), apparent-solar duration is anchored to the supplied end instant, equal-hour origin systems preserve their equal SI units, and anchored Roman/Edo seasonal intervals are integrated across their real day/night unit lengths, while unanchored accumulated quantities remain explicitly SI rather than inventing a false seasonal conversion.

## Common 1.19.25

1.19.25 centralises the complete explicit clock-mode formatter behind the shared temporal catalogue. Consumers can now render the same decimal, Internet, binary/hexadecimal, astronomical, Roman/Japanese seasonal and solar-origin clocks from one portable Common contract while leaving the OS-locale `standard` mode with the platform adapter. Location-dependent modes refuse to invent geographic context.

## Common 1.19.24

1.19.24 hardens the existing graphics primitives without changing their API. Rectangle and blit ranges are clipped before endpoint arithmetic, aliased source/destination operations use snapshot semantics, and nearest-neighbour scaling reuses Common's exact overflow-safe cycle partitioning rather than constructing a potentially overflowing coordinate product.

## Common 1.19.23

1.19.23 promotes the existing exact local-civil microsecond-of-day calculation from a private temporal helper to a public Common primitive. The algorithm remains unchanged: it uses negative-safe day phases for both the Unix instant and caller-supplied UTC offset, and the existing clock formatter now consumes that same exported implementation.

## Common 1.19.22

1.19.22 hardens existing POSIX I/O without adding API surface. Bounded text reads now reject embedded NUL data using the actual byte count, temporal persistence consumes that single bounded-reader implementation, and durable unlink/atomic replacement retain one parent-directory descriptor through namespace mutation and durability sync. Linux temporary descriptors gain atomic close-on-exec creation while other POSIX targets retain the portable fallback, and regression coverage now proves atomic replacement does not follow a target symlink. The existing `infiltratr_mkdir_parents()` contract is deliberately unchanged.

## Common 1.19.21

1.19.21 strengthens existing Common behaviour without adding any public API. Current temporal-policy parsing now rejects incomplete or duplicate v3 authority fields, fixed-size temporal IDs are validated before C-string lookup, the POSIX temporal loader/provider marker use bounded reads while preserving embedded-NUL rejection, and exact cycle partitioning takes a direct checked-multiply path when the product fits before falling back to the existing overflow-safe wide algorithm.

## Common 1.19.20

1.19.20 hardens the POSIX temporal authority boundary. Provider discovery now validates the marker's provider, policy-version and contract fields rather than trusting pathname existence alone; staged/custom-prefix qualification can supply an explicit absolute marker path. Temporal policy persistence rejects non-finite or out-of-range coordinates before writing, and policy/XDG path construction no longer carries a private 4096-byte ceiling.

## Common 1.19.19

1.19.19 centralises the Linux/POSIX persistence side of temporal policy. Common now owns the canonical XDG `presentation.conf` path, validated load/atomic save helpers and the installed `temporal-v3` provider capability marker used by consumers to decide whether System Settings is authoritative. Platform-specific fallback policy remains with the consuming desktop adapter.

## Common 1.19.18

1.19.18 makes the current temporal authority a clean one-calendar contract. The retired secondary-calendar model and public v2 temporal ABI are removed. Current System Settings and Calendar consumers use policy v3 directly. A small private parser accepts already-written v2 presentation.conf files only to preserve user state during upgrade; it is not a compatibility API.

## Common 1.19.17

1.19.17 continues the bidirectional System Monitor ownership pass. Common now
owns the generic monotonic unsigned-counter delta operation beneath its existing
rate contract, allowing consumers to preserve domain-specific fallback policy
without repeating rollback/subtraction mechanics. The POSIX provider also owns
normalized absolute clock-deadline construction and upward-rounded remaining
millisecond conversion, replacing repeated `timespec` normalization and timeout
arithmetic while leaving pthread/device scheduling policy in the consumer.

## Common 1.19.16

1.19.16 completes the system-wide temporal presentation authority started in
1.19.14. The shared policy now carries the complete Calendar-proven clock
catalogue, primary and secondary calendar systems, seconds policy and optional
geographic latitude/longitude. System Settings is the authority, so the global
clock catalogue deliberately contains "Standard time (OS locale)" rather than a
self-referential "Follow system" mode. Version-1 policy files migrate in memory
to the version-2 schema.

## Common 1.19.15

1.19.15 extends the System Monitor bidirectional ownership pass with two
portable contracts that were still duplicated or locale-sensitive in production.
The deterministic ASCII family now includes lexical case-insensitive ordering,
removing the final `strcasecmp` dependency from System Monitor's model sort.
Common also owns a stable non-cryptographic 64-bit FNV-1a byte/text/u64 mixer
for runtime identity and change signatures, replacing repeated private hashing
loops without moving product-specific signature composition into the library.

## Common 1.19.14

1.19.14 begins the shared system-wide temporal presentation contract. Common
now owns stable system/12-hour/24-hour/decimal-10 clock-profile identifiers,
a versioned toolkit-neutral policy document, and exact portable formatting for
explicit conventional and decimal clocks. The decimal implementation promotes
Calendar's proven rational civil-day partition semantics so applications can
share one 10-hour clock without changing canonical timestamps.

## Common 1.19.13

1.19.13 extends the POSIX provider with shared user-path and directory-creation
contracts proven by System Monitor. Consumers can resolve the current home
directory, XDG configuration/data homes and recursively create directory trees
without importing toolkit helpers into non-UI code. The XDG helpers reject
relative overrides and follow the base-directory specification's HOME
fallbacks. The same consumer pass completes Common's deterministic public ASCII
family with classification and case-insensitive substring matching so kernel,
protocol and identifier grammars do not fall back to locale/toolkit helpers.

## Common 1.19.12

1.19.12 continues the bidirectional consumer pass by promoting deterministic
ASCII case conversion/comparison that Common already used internally and by
completing the POSIX lexical-path pair with a bounded dirname operation.
System Monitor can therefore use one shared contract for hardware/protocol
identifiers and parent-directory extraction instead of POSIX locale helpers or
GTK allocation helpers.

## Common 1.19.11

1.19.11 completes two consumer-proven Common contracts found during the
bidirectional System Monitor audit. Theme policy now owns canonical lowercase
persistence keys and strict case-insensitive parsing for System/Day/Night,
eliminating consumer-local serialization tables. Shared metric formatting now
also provides the two-decimal GHz presentation already used by System Monitor,
so consumers no longer need a private scalar-format wrapper. The existing
atomic dynamic-library symbol-table binder and canonical build-profile label
remain the authoritative mechanisms for consumers that had retained local
loops or label tables.

## Common 1.19.10

1.19.10 makes the Linux MBLINK face the exact Night reference instead of
reducing it to one replacement grey. The canonical canvas returns to
`#050608`, while the surrounding graphite layers remain distinct:
`#101318`, `#171B20`, `#0D1014`, `#0E1115`, `#202125` and their
associated borders and text roles. The native C palette and web/JSON adapters
now also expose the visible MBLINK Linux titlebar, connection-bar, heading,
summary, kicker, detail, note, status-border, hover-accent and state-border
colours so consumers can reproduce the layered dark face instead of flattening
Night into black or a single grey.

## Common 1.19.8

1.19.8 removes the remaining clear implementation duplication found by the
forensic pass without changing the public ABI. Whole signed and unsigned
integer parsers now reuse the cursor-token parsers and add only complete-input
validation. Deterministic ASCII classification, case folding and case-insensitive
span matching live in one private internal utility. POSIX u64/i64/double file
readers share one complete-file, embedded-NUL and status-translation path.
Escaping uses Common's checked size arithmetic rather than a private overflow
helper.

## Common 1.19.7

1.19.7 completes the reusable numeric-token family with exact locale-independent
binary64 token parsing, allowing Calculator to remove its private decimal-token
scanner while retaining Calculator-owned expression grammar. The native design
adapter now exposes canonical structural metrics and typography identity, and a
shared CMake metadata file centralises immutable MB Corpo asset provenance for
consumers that bundle the verified faces.

## Common 1.19.6

The appearance contract is now explicit: Day is the white palette, Night is the MB graphite/black palette with the canonical `#00ADEF` accent, and System must resolve the host light/dark preference to exactly one of those two palettes.

1.19.6 centralises two project-family mechanics that had begun to drift across
consumers. Project build profiles now have one canonical human-readable label
contract, and the dynamic-library adapter can atomically bind a table of
required and optional symbols without each application carrying its own
required-symbol loop. Calendar consumes both contracts immediately while
retaining ICU-specific version probing and all chronology policy locally.

## Common 1.19.4

1.19.4 removes three remaining generic mechanisms from System Monitor. Common
now exposes complete dynamically allocated POSIX text-file reads, deterministic
locale-independent fixed-point ASCII formatting for persisted numeric data, and
an always-quoted spreadsheet-safe CSV field encoder. The new contracts retain
explicit size/failure semantics and are covered by the existing POSIX,
formatting and escaping regression suites.

## Common 1.19.3

1.19.3 completes Common's signed saturating-arithmetic family with
`infiltratr_i64_multiply_saturating()`. The implementation is defined in
terms of Common's checked signed multiplication contract, clamps positive and
negative overflow to the nearest signed endpoint, and replaces Calendar's last
private generic signed-overflow helper.

## Common 1.19.2

1.19.2 makes the shared unit policy explicit: memory and storage remain binary
(base-2) while network quantities and negotiated link rates use decimal
1000-based Kb/Mb/Gb scaling. The network regression fixtures now use exact
decimal boundaries so binary scaling cannot pass accidentally through display
rounding.

## Common 1.19.1

1.19.1 centralises the shared appearance contract. Common now defines
System/Day/Night theme policy, canonical semantic Day and Night palettes, a
portable C adapter for native consumers, and a web adapter that follows the
same values. System remains platform-authoritative rather than becoming a
third invented palette. The historical top-level JSON palette and default web
variables remain the Night palette for backward compatibility, so existing
consumers do not change appearance merely by updating Common.

## Common 1.19.0

1.19.0 promotes smooth software-framebuffer scaling into Common. The graphics
surface API now provides whole-surface and source-region bilinear blits with
premultiplied-alpha interpolation, and source-over compositing now preserves
destination alpha correctly. Backyard Racer can therefore remove its private
smooth-scaling implementation while keeping pixel-art paths on nearest-neighbour
scaling. The graphics regression suite covers transparent-edge filtering and
alpha preservation.

## Common 1.18.1

1.18.1 is a source-package hygiene patch. It adds the missing GPL-3 SPDX
markers to the 1.18 web-design validator and reusable Pages action and adds a
repository-wide SPDX regression gate. Runtime library behaviour and the public
ABI are unchanged from 1.18.0.

## Common 1.18.0

1.18.0 extends Common into the shared static-web boundary without moving product
semantics into the library. It adds dependency-free HTML/JSON/URI output
encoders, the canonical web design-token adapter, and a reusable GitHub Pages
artifact/deployment action. Native generators can combine these portable
encoders with Common's existing durable POSIX atomic-file publication instead
of carrying private copies.

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

Copyright © 1993-2026 Shannon Smith.

Common is licensed under GNU GPL-3.0-or-later. See `LICENSE`.
