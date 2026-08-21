// SPDX-License-Identifier: GPL-3.0-or-later
#include "infiltratr/core.h"

#include <stdio.h>
#include <string.h>

int main(void)
{
    if (strcmp(INFILTRATR_COMMON_VERSION, "1.10.0") != 0) {
        fputs("unexpected Infiltratr Common version\n", stderr);
        return 1;
    }

    puts("Infiltratr Common package consumer passed");
    return 0;
}
