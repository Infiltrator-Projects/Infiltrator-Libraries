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

The common visual language is a graphite/silver foundation: near-black application background, layered dark panels/cards, restrained silver borders, light foreground text and muted secondary text. Controls use the same typography and geometry across products.

`infiltrator-web-v1.css` is the canonical web adapter for these neutral roles. It exposes stable CSS custom properties for the shared palette, typography roles, radii and spacing. It contains no product page layout, no manufacturer identity and no proprietary font binaries. Web consumers load the adapter first and layer genuine local identity afterwards.

Product repositories continue to own identity. Manufacturer colours, product accents, icons, logos, illustrations, vehicle gauges and domain-specific components are not part of the shared contract. A product may override the neutral accent while retaining the common typography, structural palette and component metrics.

## Ownership

This directory is a design contract, not part of the Infiltratr Common C ABI. Runtime algorithms and broadly reusable C mechanics remain in Common. LINK remains the source of truth for shared vehicle-diagnostics behaviour and shared LINK-family application shells.

When a platform cannot consume the JSON directly, its adapter should mirror these named roles rather than invent a new typography or structural palette. The web adapter is maintained alongside the JSON and regression-checked against its named values so the two cannot silently diverge. Product-specific source should contain only genuine identity overrides.

## Change rule

A design-token change is made here first and then propagated to graphical consumers. Consumer-specific exceptions must be intentional and documented; silent drift is a bug.

SPDX-License-Identifier: GPL-3.0-or-later
