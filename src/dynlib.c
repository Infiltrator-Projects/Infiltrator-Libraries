// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file dynlib.c
 * @brief POSIX/Win32 implementation of the shared dynamic-library boundary.
 *
 * @author Shannon Smith
 * @copyright Copyright (c) 2026 Shannon Smith
 * @license GPL-3.0-or-later
 */
#include "infiltratr/dynlib.h"

#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

bool infiltratr_dynlib_open(InfiltratrDynlib *library, const char *name)
{
    if (library == NULL || name == NULL || name[0] == '\0' ||
        library->handle != NULL) {
        return false;
    }

#ifdef _WIN32
    library->handle = (void *)LoadLibraryA(name);
#else
    library->handle = dlopen(name, RTLD_NOW | RTLD_LOCAL);
#endif
    return library->handle != NULL;
}

bool infiltratr_dynlib_is_open(const InfiltratrDynlib *library)
{
    return library != NULL && library->handle != NULL;
}

void infiltratr_dynlib_close(InfiltratrDynlib *library)
{
    if (library == NULL || library->handle == NULL) {
        return;
    }

#ifdef _WIN32
    (void)FreeLibrary((HMODULE)library->handle);
#else
    (void)dlclose(library->handle);
#endif
    library->handle = NULL;
}

bool infiltratr_dynlib_symbol(const InfiltratrDynlib *library,
                              const char *name,
                              void *destination,
                              size_t destination_size)
{
    void *symbol = NULL;

    if (!infiltratr_dynlib_is_open(library) || name == NULL ||
        name[0] == '\0' || destination == NULL ||
        destination_size != sizeof(symbol)) {
        return false;
    }

#ifdef _WIN32
    {
        const FARPROC procedure =
            GetProcAddress((HMODULE)library->handle, name);

        _Static_assert(sizeof(symbol) == sizeof(procedure),
                       "Win32 function and data pointers must have equal size");
        if (procedure == NULL) {
            return false;
        }
        memcpy(&symbol, &procedure, sizeof(symbol));
    }
#else
    symbol = dlsym(library->handle, name);
    if (symbol == NULL) {
        return false;
    }
#endif

    memcpy(destination, &symbol, sizeof(symbol));
    return true;
}
