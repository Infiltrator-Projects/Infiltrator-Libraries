// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file posix_io_contract.c
 * @brief Exact positioned POSIX I/O regression coverage.
 */
#define _POSIX_C_SOURCE 200809L

#include "infiltratr/posix_io.h"

#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(void)
{
    char path[] = "infiltratr-posix-io-XXXXXX";
    const int descriptor = mkstemp(path);
    assert(descriptor >= 0);
    assert(unlink(path) == 0);
    assert(ftruncate(descriptor, 16) == 0);

    static const char payload[] = "hello";
    assert(infiltratr_pwrite_full(descriptor, payload,
                                  sizeof(payload) - 1U, 4U) == 0);

    char readback[sizeof(payload)] = {0};
    assert(infiltratr_pread_full(descriptor, readback,
                                 sizeof(payload) - 1U, 4U) == 0);
    assert(memcmp(readback, payload, sizeof(payload) - 1U) == 0);

    assert(infiltratr_pread_full(descriptor, NULL, 0U, 0U) == 0);
    assert(infiltratr_pwrite_full(descriptor, NULL, 0U, 0U) == 0);

    errno = 0;
    char short_read[4] = {0};
    assert(infiltratr_pread_full(descriptor, short_read,
                                 sizeof(short_read), 15U) == -1);
    assert(errno == EIO);

    errno = 0;
    assert(infiltratr_pread_full(descriptor, readback, 1U,
                                 UINT64_C(0x8000000000000000)) == -1);
    assert(errno == EOVERFLOW);

    errno = 0;
    assert(infiltratr_pwrite_full(descriptor, payload, 1U,
                                  UINT64_C(0x8000000000000000)) == -1);
    assert(errno == EOVERFLOW);

    errno = 0;
    assert(infiltratr_pread_full(descriptor, NULL, 1U, 0U) == -1);
    assert(errno == EINVAL);

    assert(close(descriptor) == 0);
    puts("Infiltratr Common exact positioned I/O contract tests passed.");
    return 0;
}
