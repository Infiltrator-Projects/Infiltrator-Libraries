# Infiltrator Design

`design/infiltrator-design-v1.json` is the canonical visual contract for graphical Infiltrator software.

The reference implementation is MBLINK. The contract deliberately extracts the reusable parts of MBLINK's presentation rather than Mercedes-specific identity.

## Typography

Owned graphical interfaces use these roles when the faces are available:

- normal UI: `MB Corpo S Title WEB`, weight 400;
- emphasis, actions and section headings: `MB Corpo S Title WEB`, weight 700;
- product/brand display titles: `MB Corpo A Title Cond WEB`, weight 400.

The canonical private/local filenames understood by existing applications are:

- `mb_corpo_a_cond_regular.ttf`;
- `mb_corpo_s_bold.ttf`;
- `mb_corpo_s_regular.ttf`.

Applications must provide a normal platform fallback when those faces are unavailable. This design contract does not grant redistribution rights and does not contain proprietary font binaries.

## Shared structure

The common visual language now has two explicit Infiltrator palettes: **Night**, the established graphite/silver foundation, and **Day**, its light counterpart. Both expose the same semantic roles so controls, states and hierarchy remain recognisably Infiltrator without forcing one luminance. **System** is not a third palette: it is a policy telling the platform adapter to follow the operating system's current light/dark preference.

`infiltrator-web-v1.css` is the canonical web adapter for these neutral roles. Existing consumers keep the historical Night values at `:root`; theme-aware consumers set `data-infiltratr-theme="system|day|night"` on the root element. System mode uses `prefers-color-scheme`. The C adapter in `include/infiltratr/design.h` exposes the same Day/Night semantic tokens to native C/C++ consumers while leaving operating-system theme detection to the platform layer.

Product repositories continue to own identity. Manufacturer colours, product accents, icons, logos, illustrations, vehicle gauges and domain-specific components are not part of the shared contract. A product may override the neutral accent while retaining the common typography, structural palette and component metrics.

## Ownership

The JSON remains the canonical design source. A deliberately small, product-neutral C adapter is published by Common so native consumers do not need private copies of palette constants; it does not own toolkit integration or product-specific styling. Runtime algorithms and broadly reusable C mechanics remain in Common. LINK remains the source of truth for shared vehicle-diagnostics behaviour and shared LINK-family application shells.

When a platform cannot consume the JSON directly, its adapter should mirror these named roles rather than invent a new typography or structural palette. The web adapter is maintained alongside the JSON and regression-checked against its named values so the two cannot silently diverge. Product-specific source should contain only genuine identity overrides.

## Change rule

A design-token change is made here first. The JSON, C adapter and web adapter are regression-checked against one another before consumers move. Consumer-specific exceptions must be intentional and documented; silent drift is a bug. Platform CSS may differ in selector syntax and widget mechanics, but semantic roles and canonical colour values should not.

SPDX-License-Identifier: GPL-3.0-or-later
