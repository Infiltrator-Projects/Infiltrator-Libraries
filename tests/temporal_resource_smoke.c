// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
/**
 * @file temporal_resource_smoke.c
 * @brief Contract smoke test for filesystem temporal-resource discovery.
 */
#include "infiltratr/temporal_resource.h"

#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#include <process.h>
#define TEST_MKDIR(path) _mkdir(path)
#define TEST_RMDIR(path) _rmdir(path)
#define TEST_PID() ((long)_getpid())
#else
#include <sys/stat.h>
#include <unistd.h>
#define TEST_MKDIR(path) mkdir((path), 0700)
#define TEST_RMDIR(path) rmdir(path)
#define TEST_PID() ((long)getpid())
#endif

typedef struct Capture {
    size_t count;
    InfiltratrTemporalResourceKind kinds[8];
    InfiltratrTemporalResourceForm forms[8];
    char names[8][64];
} Capture;

static bool capture_resource(const InfiltratrTemporalResourceInfo *resource,
                             void *user_data)
{
    Capture *capture = user_data;
    size_t index;

    assert(resource != NULL);
    assert(resource->struct_size == sizeof(*resource));
    assert(resource->abi_version == INFILTRATR_TEMPORAL_RESOURCE_INFO_ABI);
    assert(resource->name != NULL && resource->name[0] != '\0');
    assert(resource->path != NULL && resource->path[0] != '\0');
    assert(capture != NULL && capture->count < 8U);

    index = capture->count++;
    capture->kinds[index] = resource->kind;
    capture->forms[index] = resource->form;
    assert(strlen(resource->name) < sizeof(capture->names[index]));
    strcpy(capture->names[index], resource->name);
    return true;
}

static void join_test_path(char *buffer, size_t capacity,
                           const char *left, const char *right)
{
    const int written = snprintf(buffer, capacity, "%s/%s", left, right);
    assert(written > 0 && (size_t)written < capacity);
}

static void make_file(const char *path)
{
    FILE *stream = fopen(path, "wb");
    assert(stream != NULL);
    assert(fputs("fixture\n", stream) >= 0);
    assert(fclose(stream) == 0);
}

int main(void)
{
    char root[128];
    char calendars[192];
    char clocks[192];
    char path[256];
    char missing[192];
    Capture capture = {0};
    size_t count = SIZE_MAX;
    int written;

    written = snprintf(root, sizeof(root), "temporal-resource-smoke-%ld", TEST_PID());
    assert(written > 0 && (size_t)written < sizeof(root));
    join_test_path(calendars, sizeof(calendars), root, "calendars");
    join_test_path(clocks, sizeof(clocks), root, "clocks");
    join_test_path(missing, sizeof(missing), root, "missing");

    assert(TEST_MKDIR(root) == 0);
    assert(TEST_MKDIR(calendars) == 0);
    assert(TEST_MKDIR(clocks) == 0);

    join_test_path(path, sizeof(path), calendars, "zeta");
    make_file(path);
    join_test_path(path, sizeof(path), calendars, "alpha");
    make_file(path);
    join_test_path(path, sizeof(path), calendars, "middle");
    assert(TEST_MKDIR(path) == 0);
    join_test_path(path, sizeof(path), calendars, ".ignored");
    make_file(path);
    join_test_path(path, sizeof(path), clocks, "one");
    make_file(path);

#ifndef _WIN32
    join_test_path(path, sizeof(path), calendars, "link");
    assert(symlink("alpha", path) == 0);
#endif

    assert(!infiltratr_temporal_resource_scan_directory(
        NULL, INFILTRATR_TEMPORAL_RESOURCE_CALENDAR,
        capture_resource, &capture, &count));
    assert(count == SIZE_MAX);
    assert(infiltratr_temporal_resource_scan_directory(
        missing, INFILTRATR_TEMPORAL_RESOURCE_CALENDAR,
        capture_resource, &capture, &count));
    assert(count == 0U);
    assert(capture.count == 0U);

    assert(infiltratr_temporal_resource_scan_root(
        root, capture_resource, &capture, &count));
    assert(count == 4U);
    assert(capture.count == 4U);
    assert(capture.kinds[0] == INFILTRATR_TEMPORAL_RESOURCE_CALENDAR);
    assert(capture.kinds[1] == INFILTRATR_TEMPORAL_RESOURCE_CALENDAR);
    assert(capture.kinds[2] == INFILTRATR_TEMPORAL_RESOURCE_CALENDAR);
    assert(capture.kinds[3] == INFILTRATR_TEMPORAL_RESOURCE_CLOCK);
    assert(strcmp(capture.names[0], "alpha") == 0);
    assert(strcmp(capture.names[1], "middle") == 0);
    assert(strcmp(capture.names[2], "zeta") == 0);
    assert(strcmp(capture.names[3], "one") == 0);
    assert(capture.forms[0] == INFILTRATR_TEMPORAL_RESOURCE_FILE);
    assert(capture.forms[1] == INFILTRATR_TEMPORAL_RESOURCE_DIRECTORY);
    assert(capture.forms[2] == INFILTRATR_TEMPORAL_RESOURCE_FILE);
    assert(capture.forms[3] == INFILTRATR_TEMPORAL_RESOURCE_FILE);

    count = SIZE_MAX;
    assert(infiltratr_temporal_resource_scan_root(root, NULL, NULL, &count));
    assert(count == 4U);

#ifndef _WIN32
    join_test_path(path, sizeof(path), calendars, "link");
    assert(remove(path) == 0);
#endif
    join_test_path(path, sizeof(path), calendars, ".ignored");
    assert(remove(path) == 0);
    join_test_path(path, sizeof(path), calendars, "alpha");
    assert(remove(path) == 0);
    join_test_path(path, sizeof(path), calendars, "zeta");
    assert(remove(path) == 0);
    join_test_path(path, sizeof(path), calendars, "middle");
    assert(TEST_RMDIR(path) == 0);
    join_test_path(path, sizeof(path), clocks, "one");
    assert(remove(path) == 0);
    assert(TEST_RMDIR(calendars) == 0);
    assert(TEST_RMDIR(clocks) == 0);
    assert(TEST_RMDIR(root) == 0);
    return 0;
}
