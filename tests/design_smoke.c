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
    if (day->background_rgb != 0xFFFFFFU) return 4;
    if (night->background_rgb != 0x050608U) return 5;
    if (day->button_background_rgb != 0x20252BU) return 6;
    if (night->button_background_rgb != 0xD7DDE2U) return 7;
    if (day->neutral_accent_rgb != 0x00ADEFU) return 11;
    if (night->neutral_accent_rgb != 0x00ADEFU) return 12;
    if (system_light != day || system_dark != night) return 8;
    if (strcmp(infiltratr_theme_mode_name(INFILTRATR_THEME_SYSTEM), "System") != 0) return 9;
    if (infiltratr_theme_mode_next(INFILTRATR_THEME_SYSTEM) != INFILTRATR_THEME_DAY) return 10;
    if (infiltratr_theme_mode_next(INFILTRATR_THEME_DAY) != INFILTRATR_THEME_NIGHT) return 11;
    if (infiltratr_theme_mode_next(INFILTRATR_THEME_NIGHT) != INFILTRATR_THEME_SYSTEM) return 12;

    const InfiltratrDesignMetrics *metrics = infiltratr_design_metrics();
    if (metrics == NULL || metrics->abi_version != INFILTRATR_DESIGN_METRICS_ABI) return 13;
    if (metrics->control_radius != 10U || metrics->card_radius != 12U) return 14;
    if (metrics->compact_spacing != 6U || metrics->content_padding != 16U) return 15;

    const InfiltratrTypography *type = infiltratr_typography();
    if (type == NULL || type->abi_version != INFILTRATR_TYPOGRAPHY_ABI) return 16;
    if (strcmp(type->ui_family, "MB Corpo S Title WEB") != 0) return 17;
    if (strcmp(type->brand_family, "MB Corpo A Title Cond WEB") != 0) return 18;
    if (type->ui_regular_weight != 400U || type->ui_bold_weight != 700U ||
        type->brand_weight != 400U) return 19;
    if (strcmp(type->brand_regular_filename, "mb_corpo_a_cond_regular.ttf") != 0 ||
        strcmp(type->ui_bold_filename, "mb_corpo_s_bold.ttf") != 0 ||
        strcmp(type->ui_regular_filename, "mb_corpo_s_regular.ttf") != 0) return 20;
    return 0;
}
