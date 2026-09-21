// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file temporal_posix.c
 * @brief POSIX temporal policy persistence and provider capability.
 */
#include "infiltratr/temporal_posix.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define TEMPORAL_PATH_CAPACITY 4096U
#define TEMPORAL_DOCUMENT_CAPACITY 1024U

bool infiltratr_temporal_posix_policy_directory(char *destination,
                                                size_t size)
{
    char config_home[TEMPORAL_PATH_CAPACITY];

    if (destination == NULL || size == 0U) {
        return false;
    }
    destination[0] = '\0';
    return infiltratr_xdg_config_home(config_home, sizeof(config_home)) &&
           infiltratr_path_join(destination, size, config_home, "infiltrator");
}

bool infiltratr_temporal_posix_policy_path(char *destination, size_t size)
{
    char directory[TEMPORAL_PATH_CAPACITY];

    if (destination == NULL || size == 0U) {
        return false;
    }
    destination[0] = '\0';
    return infiltratr_temporal_posix_policy_directory(
               directory, sizeof(directory)) &&
           infiltratr_path_join(
               destination, size, directory, "presentation.conf");
}

bool infiltratr_temporal_posix_provider_marker_path(char *destination,
                                                    size_t size)
{
    const size_t length = strlen(INFILTRATR_TEMPORAL_PROVIDER_MARKER);

    if (destination == NULL || size == 0U) {
        return false;
    }
    destination[0] = '\0';
    if (length >= size) {
        return false;
    }
    memcpy(destination, INFILTRATR_TEMPORAL_PROVIDER_MARKER, length + 1U);
    return true;
}

bool infiltratr_temporal_posix_provider_available(void)
{
    return access(INFILTRATR_TEMPORAL_PROVIDER_MARKER, R_OK) == 0;
}

InfiltratrIoResult infiltratr_temporal_posix_policy_load(
    InfiltratrTemporalPolicyV3 *policy,
    bool *found)
{
    char path[TEMPORAL_PATH_CAPACITY];
    char *text = NULL;
    size_t length = 0U;
    InfiltratrIoResult result;

    if (policy == NULL || found == NULL ||
        !infiltratr_temporal_policy_v3_default(policy) ||
        !infiltratr_temporal_posix_policy_path(path, sizeof(path))) {
        return INFILTRATR_IO_INVALID_ARGUMENT;
    }

    *found = false;
    result = infiltratr_read_text_file_alloc(path, &text, &length);
    if (result == INFILTRATR_IO_NOT_FOUND) {
        return INFILTRATR_IO_OK;
    }
    if (result != INFILTRATR_IO_OK) {
        free(text);
        return result;
    }

    if (text == NULL || strlen(text) != length ||
        !infiltratr_temporal_policy_v3_parse(text, policy)) {
        free(text);
        (void)infiltratr_temporal_policy_v3_default(policy);
        return INFILTRATR_IO_INVALID_VALUE;
    }

    free(text);
    *found = true;
    return INFILTRATR_IO_OK;
}

int infiltratr_temporal_posix_policy_save(
    const InfiltratrTemporalPolicyV3 *policy)
{
    char directory[TEMPORAL_PATH_CAPACITY];
    char path[TEMPORAL_PATH_CAPACITY];
    char document[TEMPORAL_DOCUMENT_CAPACITY];
    size_t length = 0U;
    int failure;

    if (policy == NULL ||
        !infiltratr_temporal_posix_policy_directory(
            directory, sizeof(directory)) ||
        !infiltratr_temporal_posix_policy_path(path, sizeof(path)) ||
        !infiltratr_temporal_policy_v3_serialize(
            policy, document, sizeof(document), &length)) {
        return EINVAL;
    }

    failure = infiltratr_mkdir_parents(directory, 0700U);
    if (failure != 0) {
        return failure;
    }
    return infiltratr_atomic_file_write_bytes(
        path, INFILTRATR_ATOMIC_FILE_PRIVATE, document, length);
}
