// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * @file design.h
 * @brief Product-neutral Infiltrator theme contract for native consumers.
 *
 * The platform adapter owns detection of the operating-system appearance.
 * Common owns only the stable System/Day/Night policy and semantic Day/Night
 * palette values. Product accents and domain-specific colours remain local.
 */
#ifndef INFILTRATR_COMMON_DESIGN_H
#define INFILTRATR_COMMON_DESIGN_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define INFILTRATR_THEME_PALETTE_ABI 1U

typedef enum {
    INFILTRATR_THEME_SYSTEM = 0,
    INFILTRATR_THEME_DAY = 1,
    INFILTRATR_THEME_NIGHT = 2
} InfiltratrThemeMode;

typedef struct {
    size_t struct_size;
    uint32_t abi_version;
    uint32_t background_rgb;
    uint32_t panel_rgb;
    uint32_t card_rgb;
    uint32_t surface_rgb;
    uint32_t input_rgb;
    uint32_t border_rgb;
    uint32_t text_rgb;
    uint32_t title_rgb;
    uint32_t muted_rgb;
    uint32_t subtle_rgb;
    uint32_t button_background_rgb;
    uint32_t button_foreground_rgb;
    uint32_t selection_background_rgb;
    uint32_t selection_foreground_rgb;
    uint32_t neutral_accent_rgb;
    uint32_t success_rgb;
    uint32_t warning_rgb;
    uint32_t fault_rgb;
    uint32_t info_rgb;
    uint32_t operation_rgb;
    uint32_t card_hover_rgb;
    uint32_t surface_hover_rgb;
    uint32_t operation_hover_rgb;
    uint32_t equals_hover_rgb;
} InfiltratrThemePalette;

const char *infiltratr_theme_mode_name(InfiltratrThemeMode mode);
InfiltratrThemeMode infiltratr_theme_mode_next(InfiltratrThemeMode mode);
const InfiltratrThemePalette *infiltratr_theme_resolve(
    InfiltratrThemeMode mode, bool system_is_dark);

#ifdef __cplusplus
}
#endif

#endif
