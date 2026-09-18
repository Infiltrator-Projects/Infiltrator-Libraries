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

for key in background panel card surface input border text title muted subtle \
  button_background button_foreground selection_background selection_foreground \
  neutral_accent success warning fault info; do
  css_key=${key//_/-}
  check_string ".palette.$key" "--infiltratr-$css_key: " ';'
done

for key in small_radius control_radius card_radius panel_radius compact_spacing \
  control_spacing section_spacing content_padding screen_padding; do
  css_key=${key//_/-}
  check_string ".metrics.$key" "--infiltratr-$css_key: " 'px;'
done

echo "PASS: web CSS adapter matches canonical design JSON"
