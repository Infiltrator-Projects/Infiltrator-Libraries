#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
set -euo pipefail

root=$(cd "$(dirname "$0")/.." && pwd)
json="$root/design/infiltrator-design-v1.json"
css="$root/design/infiltrator-web-v1.css"

check_string() {
  local json_path=$1 css_name=$2 suffix=${3-}
  local value
  value=$(jq -er "$json_path" "$json")
  grep -Fq -- "$css_name$value$suffix" "$css"
}

check_string '.typography.ui_family' '--infiltratr-font-ui: "' '", sans-serif;'
check_string '.typography.brand_family' '--infiltratr-font-brand: "' '", sans-serif;'
check_string '.typography.ui_regular_weight' '--infiltratr-font-ui-regular-weight: ' ';'
check_string '.typography.ui_bold_weight' '--infiltratr-font-ui-bold-weight: ' ';'
check_string '.typography.brand_weight' '--infiltratr-font-brand-weight: ' ';'

legacy_keys=(background panel card surface input border text title muted subtle
  button_background button_foreground selection_background selection_foreground
  neutral_accent success warning fault info)
theme_keys=("${legacy_keys[@]}" operation card_hover surface_hover operation_hover equals_hover)

for key in "${legacy_keys[@]}"; do
  test "$(jq -er ".palette.$key" "$json")" = "$(jq -er ".theme.palettes.night.$key" "$json")"
done

for mode in night day; do
  for key in "${theme_keys[@]}"; do
    css_key=${key//_/-}
    value=$(jq -er ".theme.palettes.$mode.$key" "$json")
    grep -Fq -- "--infiltratr-$css_key: $value;" "$css"
  done
done

grep -Fq ':root[data-infiltratr-theme="day"]' "$css"
grep -Fq ':root[data-infiltratr-theme="night"]' "$css"
grep -Fq ':root[data-infiltratr-theme="system"]' "$css"
grep -Fq '@media (prefers-color-scheme: light)' "$css"

for key in small_radius control_radius card_radius panel_radius compact_spacing \
  control_spacing section_spacing content_padding screen_padding; do
  css_key=${key//_/-}
  check_string ".metrics.$key" "--infiltratr-$css_key: " 'px;'
done

for mode in day night; do
  for key in "${theme_keys[@]}"; do
    value=$(jq -er ".theme.palettes.$mode.$key" "$json")
    hex=${value#\#}
    field=${key}_rgb
    grep -Fq -- ".$field = 0x${hex}U" "$root/src/design.c"
  done
done
grep -Fq 'INFILTRATR_THEME_SYSTEM' "$root/include/infiltratr/design.h"
echo "PASS: JSON, C and web design adapters match the canonical theme contract"
