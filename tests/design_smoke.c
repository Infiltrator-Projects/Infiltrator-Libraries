// SPDX-License-Identifier: GPL-3.0-or-later
#include "infiltratr/design.h"

#include <assert.h>
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

    assert(day != NULL);
    assert(night != NULL);
    assert(day->abi_version == INFILTRATR_THEME_PALETTE_ABI);
    assert(night->abi_version == INFILTRATR_THEME_PALETTE_ABI);
    assert(day->background_rgb == 0xF4F5F7U);
    assert(night->background_rgb == 0x050608U);
    assert(day->button_background_rgb == 0x20252BU);
    assert(night->button_background_rgb == 0xD7DDE2U);
    assert(system_light == day);
    assert(system_dark == night);
    assert(strcmp(infiltratr_theme_mode_name(INFILTRATR_THEME_SYSTEM), "System") == 0);
    assert(infiltratr_theme_mode_next(INFILTRATR_THEME_SYSTEM) == INFILTRATR_THEME_DAY);
    assert(infiltratr_theme_mode_next(INFILTRATR_THEME_DAY) == INFILTRATR_THEME_NIGHT);
    assert(infiltratr_theme_mode_next(INFILTRATR_THEME_NIGHT) == INFILTRATR_THEME_SYSTEM);
    return 0;
}
