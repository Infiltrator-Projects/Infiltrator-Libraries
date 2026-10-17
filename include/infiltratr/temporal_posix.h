// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file temporal_posix.h
 * @brief POSIX persistence and temporal-resource adapter.
 *
 * Temporal schema/validation stays in temporal.h. This adapter owns the
 * canonical XDG persistence location and the installed-provider capability
 * marker used by POSIX desktop consumers. Date presentation is resolved through
 * Common's system Calendar Resources ABI rather than through an application.
 */
#ifndef INFILTRATR_COMMON_TEMPORAL_POSIX_H
#define INFILTRATR_COMMON_TEMPORAL_POSIX_H

#include "infiltratr/posix.h"
#include "infiltratr/temporal.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef INFILTRATR_TEMPORAL_PROVIDER_MARKER
#define INFILTRATR_TEMPORAL_PROVIDER_MARKER \
    "/usr/share/infiltrator/policy-providers/temporal-v3"
#endif

/** Resolve the per-user directory containing presentation.conf. */
bool infiltratr_temporal_posix_policy_directory(char *destination,
                                                size_t size);

/** Resolve the canonical per-user temporal policy document path. */
bool infiltratr_temporal_posix_policy_path(char *destination, size_t size);

/**
 * Resolve the installed temporal-provider capability marker path.
 *
 * The compiled default may be overridden by the absolute
 * INFILTRATR_TEMPORAL_PROVIDER_MARKER_PATH environment value. This is intended
 * for staged/custom-prefix installations and deterministic qualification.
 */
bool infiltratr_temporal_posix_provider_marker_path(char *destination,
                                                    size_t size);

/**
 * True only when the provider marker exists and declares the exact supported
 * provider, policy version and temporal contract.
 */
bool infiltratr_temporal_posix_provider_available(void);

/**
 * Load and validate the current user's temporal policy.
 *
 * Missing policy is a successful no-policy state: @found is false and @policy
 * receives v3 defaults. Malformed or unreadable policy returns a rich I/O
 * result and leaves @found false.
 */
InfiltratrIoResult infiltratr_temporal_posix_policy_load(
    InfiltratrTemporalPolicyV3 *policy,
    bool *found);

/**
 * Atomically persist a validated v3 policy in the canonical XDG location.
 *
 * Returns zero on success or an errno-style failure code.
 */
int infiltratr_temporal_posix_policy_save(
    const InfiltratrTemporalPolicyV3 *policy);

typedef struct InfiltratrTemporalDateProvider InfiltratrTemporalDateProvider;

/**
 * Create a lazy, single-thread-owned Calendar Resources date provider.
 *
 * Construction performs no filesystem discovery. On first use, Common resolves
 * the requested stable calendar ID only from the canonical native calendar
 * resource catalogue. A missing resource is retried at most once per minute for
 * the same ID so high-volume consumers do not turn absence into repeated I/O.
 */
InfiltratrTemporalDateProvider *infiltratr_temporal_posix_date_provider_new(void);

/**
 * Bind one explicit absolute Calendar Resource module for staging/tests.
 *
 * Both Common resource metadata and the executable calendar ABI must validate.
 * The resulting provider accepts only the module's own stable calendar ID. The
 * caller owns selection/trust of this explicit path.
 */
InfiltratrTemporalDateProvider *infiltratr_temporal_posix_date_provider_new_from(
    const char *library_path);

/** Release the provider and any loaded resource module. NULL is a safe no-op. */
void infiltratr_temporal_posix_date_provider_free(
    InfiltratrTemporalDateProvider *provider);

/**
 * Render a proleptic Gregorian civil date through the selected installed
 * Calendar Resource. No timezone or canonical timestamp is changed.
 *
 * Parts are "short" and "full". Invalid civil dates, absent/mismatched
 * resources, oversized output and malformed UTF-8 return false without
 * modifying caller output or length. Calendar applications are not consulted
 * and need not be installed.
 */
bool infiltratr_temporal_posix_format_date(
    InfiltratrTemporalDateProvider *provider,
    const char *calendar_id,
    int32_t gregorian_year,
    int32_t gregorian_month,
    int32_t gregorian_day,
    const char *part,
    char *buffer,
    size_t capacity,
    size_t *length);

#ifdef __cplusplus
}
#endif

#endif
