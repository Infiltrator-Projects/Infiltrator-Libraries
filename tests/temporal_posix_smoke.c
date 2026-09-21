// SPDX-License-Identifier: GPL-3.0-or-later
#define _POSIX_C_SOURCE 200809L
#include "infiltratr/temporal_posix.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(void)
{
    char template_path[] = "/tmp/infiltratr-temporal-XXXXXX";
    char *root = mkdtemp(template_path);
    char expected[4096];
    char actual[4096];
    char marker[4096];
    InfiltratrTemporalPolicyV3 policy;
    InfiltratrTemporalPolicyV3 loaded;
    bool found = true;

    assert(root != NULL);
    assert(setenv("XDG_CONFIG_HOME", root, 1) == 0);

    assert(infiltratr_temporal_posix_policy_directory(
        actual, sizeof(actual)));
    assert(snprintf(expected, sizeof(expected), "%s/infiltrator", root) > 0);
    assert(strcmp(actual, expected) == 0);

    assert(infiltratr_temporal_posix_policy_path(actual, sizeof(actual)));
    assert(snprintf(expected, sizeof(expected), "%s/infiltrator/presentation.conf",
                    root) > 0);
    assert(strcmp(actual, expected) == 0);

    assert(infiltratr_temporal_posix_provider_marker_path(
        marker, sizeof(marker)));
    assert(strcmp(marker, INFILTRATR_TEMPORAL_PROVIDER_MARKER) == 0);

    assert(infiltratr_temporal_posix_policy_load(&loaded, &found) ==
           INFILTRATR_IO_OK);
    assert(!found);
    assert(strcmp(loaded.clock_mode, "standard") == 0);
    assert(strcmp(loaded.calendar, "gregorian") == 0);

    assert(infiltratr_temporal_policy_v3_default(&policy));
    strcpy(policy.clock_mode, "standard-24");
    strcpy(policy.calendar, "hebrew");
    policy.show_seconds = true;
    policy.location_configured = true;
    policy.latitude = -36.39;
    policy.longitude = 145.36;

    assert(infiltratr_temporal_posix_policy_save(&policy) == 0);
    found = false;
    assert(infiltratr_temporal_posix_policy_load(&loaded, &found) ==
           INFILTRATR_IO_OK);
    assert(found);
    assert(strcmp(loaded.clock_mode, "standard-24") == 0);
    assert(strcmp(loaded.calendar, "hebrew") == 0);
    assert(loaded.show_seconds);
    assert(loaded.location_configured);
    assert(loaded.latitude == -36.39);
    assert(loaded.longitude == 145.36);

    assert(unlink(expected) == 0);
    {
        char directory[4096];
        assert(snprintf(directory, sizeof(directory), "%s/infiltrator", root) > 0);
        assert(rmdir(directory) == 0);
    }
    assert(rmdir(root) == 0);

    puts("Infiltratr Common POSIX temporal policy contract passed.");
    return 0;
}
