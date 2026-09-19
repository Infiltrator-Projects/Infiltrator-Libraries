// SPDX-License-Identifier: GPL-3.0-or-later
#include "infiltratr/design.h"

static const InfiltratrDesignMetrics design_metrics = {
    .struct_size = sizeof(InfiltratrDesignMetrics),
    .abi_version = INFILTRATR_DESIGN_METRICS_ABI,
    .small_radius = 6U,
    .control_radius = 10U,
    .card_radius = 12U,
    .panel_radius = 18U,
    .compact_spacing = 6U,
    .control_spacing = 10U,
    .section_spacing = 18U,
    .content_padding = 16U,
    .screen_padding = 20U
};

static const InfiltratrTypography typography = {
    .struct_size = sizeof(InfiltratrTypography),
    .abi_version = INFILTRATR_TYPOGRAPHY_ABI,
    .ui_family = "MB Corpo S Title WEB",
    .brand_family = "MB Corpo A Title Cond WEB",
    .ui_regular_weight = 400U,
    .ui_bold_weight = 700U,
    .brand_weight = 400U,
    .brand_regular_filename = "mb_corpo_a_cond_regular.ttf",
    .ui_bold_filename = "mb_corpo_s_bold.ttf",
    .ui_regular_filename = "mb_corpo_s_regular.ttf",
    .gtk_fallback = "Sans",
    .apple_fallback = "system",
    .windows_fallback = "Segoe UI"
};

static const InfiltratrThemePalette day_palette = {
    .struct_size = sizeof(InfiltratrThemePalette),
    .abi_version = INFILTRATR_THEME_PALETTE_ABI,
    .background_rgb = 0xFFFFFFU,
    .panel_rgb = 0xFFFFFFU,
    .card_rgb = 0xF8F9FAU,
    .surface_rgb = 0xECEFF2U,
    .input_rgb = 0xFFFFFFU,
    .border_rgb = 0xC7CDD3U,
    .text_rgb = 0x20252BU,
    .title_rgb = 0x111418U,
    .muted_rgb = 0x59636CU,
    .subtle_rgb = 0x737D86U,
    .button_background_rgb = 0x20252BU,
    .button_foreground_rgb = 0xFFFFFFU,
    .selection_background_rgb = 0xDDE2E7U,
    .selection_foreground_rgb = 0x111418U,
    .neutral_accent_rgb = 0x00ADEFU,
    .success_rgb = 0x3A8A58U,
    .warning_rgb = 0x9A6500U,
    .fault_rgb = 0xB54848U,
    .info_rgb = 0x467AA3U,
    .operation_rgb = 0xE8ECEFU,
    .card_hover_rgb = 0xEEF1F3U,
    .surface_hover_rgb = 0xF1F3F5U,
    .operation_hover_rgb = 0xDDE2E7U,
    .equals_hover_rgb = 0x343B42U
};

static const InfiltratrThemePalette night_palette = {
    .struct_size = sizeof(InfiltratrThemePalette),
    .abi_version = INFILTRATR_THEME_PALETTE_ABI,
    .background_rgb = 0x2B2B30U,
    .panel_rgb = 0x101318U,
    .card_rgb = 0x171B20U,
    .surface_rgb = 0x0D1014U,
    .input_rgb = 0x0E1115U,
    .border_rgb = 0x353A40U,
    .text_rgb = 0xE8ECEFU,
    .title_rgb = 0xEEF1F3U,
    .muted_rgb = 0xAEB6BDU,
    .subtle_rgb = 0x899198U,
    .button_background_rgb = 0xD7DDE2U,
    .button_foreground_rgb = 0x111418U,
    .selection_background_rgb = 0x2B3137U,
    .selection_foreground_rgb = 0xEEF1F3U,
    .neutral_accent_rgb = 0x00ADEFU,
    .success_rgb = 0x63AB7CU,
    .warning_rgb = 0xD19E47U,
    .fault_rgb = 0xC96B6BU,
    .info_rgb = 0x7FA7C9U,
    .operation_rgb = 0x20252BU,
    .card_hover_rgb = 0x22272DU,
    .surface_hover_rgb = 0x171B20U,
    .operation_hover_rgb = 0x2B3137U,
    .equals_hover_rgb = 0xEEF1F3U
};

const char *infiltratr_theme_mode_name(InfiltratrThemeMode mode)
{
    switch (mode) {
    case INFILTRATR_THEME_SYSTEM:
        return "System";
    case INFILTRATR_THEME_DAY:
        return "Day";
    case INFILTRATR_THEME_NIGHT:
        return "Night";
    default:
        return "System";
    }
}

InfiltratrThemeMode infiltratr_theme_mode_next(InfiltratrThemeMode mode)
{
    switch (mode) {
    case INFILTRATR_THEME_SYSTEM:
        return INFILTRATR_THEME_DAY;
    case INFILTRATR_THEME_DAY:
        return INFILTRATR_THEME_NIGHT;
    case INFILTRATR_THEME_NIGHT:
    default:
        return INFILTRATR_THEME_SYSTEM;
    }
}

const InfiltratrThemePalette *infiltratr_theme_resolve(
    InfiltratrThemeMode mode, bool system_is_dark)
{
    if (mode == INFILTRATR_THEME_DAY) {
        return &day_palette;
    }
    if (mode == INFILTRATR_THEME_NIGHT) {
        return &night_palette;
    }
    return system_is_dark ? &night_palette : &day_palette;
}

const InfiltratrDesignMetrics *infiltratr_design_metrics(void)
{
    return &design_metrics;
}

const InfiltratrTypography *infiltratr_typography(void)
{
    return &typography;
}
