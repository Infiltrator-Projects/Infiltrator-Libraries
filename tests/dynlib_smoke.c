// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file dynlib_smoke.c
 * @brief Contract smoke test for the native dynamic-library adapter.
 *
 * @author Shannon Smith
 * @copyright Copyright (c) 2026 Shannon Smith
 * @license GPL-3.0-or-later
 */
#include "infiltratr/dynlib.h"

#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <math.h>

int main(void)
{
    InfiltratrDynlib library = INFILTRATR_DYNLIB_INIT;
    double (*cosine)(double) = NULL;

#ifdef _WIN32
    const char *library_name = "msvcrt.dll";
    const char *symbol_name = "cos";
#else
    const char *library_name = "libm.so.6";
    const char *symbol_name = "cos";
#endif

    assert(!infiltratr_dynlib_is_open(&library));
    assert(!infiltratr_dynlib_open(NULL, library_name));
    assert(!infiltratr_dynlib_open(&library, ""));
    assert(infiltratr_dynlib_open(&library, library_name));
    assert(infiltratr_dynlib_is_open(&library));
    assert(!infiltratr_dynlib_open(&library, library_name));
    assert(infiltratr_dynlib_symbol(&library,
                                    symbol_name,
                                    &cosine,
                                    sizeof(cosine)));
    assert(cosine != NULL);
    assert(fabs(cosine(0.0) - 1.0) < 1.0e-12);
    assert(!infiltratr_dynlib_symbol(&library,
                                     "infiltratr_symbol_that_does_not_exist",
                                     &cosine,
                                     sizeof(cosine)));

    infiltratr_dynlib_close(&library);
    assert(!infiltratr_dynlib_is_open(&library));
    infiltratr_dynlib_close(&library);
    return 0;
}
