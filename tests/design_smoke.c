// SPDX-License-Identifier: GPL-3.0-or-later
#include "infiltratr/design.h"

#include <string.h>

int main(void)
{
    const InfiltratrThemePalette *day =
        infiltratr_theme_resolve(INFILTRATR_THEME_DAY, true);
    const InfiltratrThemePalette *night =
        infiltratr_theme_resolve(INFILTRATR_THEME_NIGHT, false);
    const InfiltratrThemePalette *system_light =
        infiltratr_theme_resolve(INFILTRATR_THEME_SYSTEM, false);
    const InfiltratrThemePalette *system_dark =
        infiltratr_theme_resolve(INFILTRATR_THEME_SYSTEM, true);

    if (day == NULL || night == NULL) return 1;
    if (day->abi_version != INFILTRATR_THEME_PALETTE_ABI) return 2;
    if (night->abi_version != INFILTRATR_THEME_PALETTE_ABI) return 3;
    if (day->background_rgb != 0xF4F5F7U) return 4;
    if (night->background_rgb != 0x050608U) return 5;
    if (day->button_background_rgb != 0x20252BU) return 6;
    if (night->button_background_rgb != 0xD7DDE2U) return 7;
    if (system_light != day || system_dark != night) return 8;
    if (strcmp(infiltratr_theme_mode_name(INFILTRATR_THEME_SYSTEM), "System") != 0) return 9;
    if (infiltratr_theme_mode_next(INFILTRATR_THEME_SYSTEM) != INFILTRATR_THEME_DAY) return 10;
    if (infiltratr_theme_mode_next(INFILTRATR_THEME_DAY) != INFILTRATR_THEME_NIGHT) return 11;
    if (infiltratr_theme_mode_next(INFILTRATR_THEME_NIGHT) != INFILTRATR_THEME_SYSTEM) return 12;
    return 0;
}
