# Infiltrator Design consumers

This ledger tracks graphical consumers of `infiltrator-design-v1.json`.

`InfiltratorFS` is intentionally excluded from this migration while its active structural/UI work is in progress.

| Repository | UI surface | Typography state | Design ownership |
| --- | --- | --- | --- |
| MBLINK | GTK / SwiftUI | Reference implementation | Product accent/identity local; shared structure through LINK |
| Calendar | Cinnamon / GTK | MB Corpo roles implemented with fallback | Calendar identity local |
| Defragmenter | GTK / Cairo | MB Corpo provisioned/used; residual Cairo font alignment pending | Defragger identity local |
| Runner Monitor | Tk | MB Corpo roles declared with fallback/private local discovery | Runner Monitor identity local |
| LINK | GTK / Win32 / SwiftUI | Shared shells; typography convergence in progress | Shared vehicle application shell |
| JAGLINK | LINK + SwiftUI | LINK geometry; MB typography migration pending | Jaguar identity local |
| BMWLINK | LINK + SwiftUI | LINK geometry; MB typography migration pending | BMW identity local |
| AUDILINK | LINK + SwiftUI | LINK geometry; MB typography migration pending | Audi identity local |
| FORDLINK | LINK + SwiftUI | LINK geometry; MB typography migration pending | Ford identity local |
| System-Monitor | GTK | MB Corpo roles applied at the application shell | System Monitor identity local |
| Character-Profiler | SwiftUI | System typography; migration pending | Character Profiler accent/identity local |
| Keeper-Group-Export | Tk/ttk | MB Corpo roles and graphite/silver structure applied | Keeper accent/identity local |
| WheresWally | Zabbix web widget | MB Corpo/design override applied; operational colours retained | NPS operational colour semantics local |
| Intune-Zabbix-Bridge | Zabbix web widget | MB Corpo and graphite/silver structure applied | Intune operational colour semantics local |
| ssmithnet.net | Static web / C++ generator | Common web roles with local MB Corpo assets and site-specific material treatment | Personal/site identity, layout and graphics local |
| Package Repository | Static web / C++ publisher | Common web roles; repository status semantics remain local | Repository/site identity and APT status presentation local |
| ExtFS-for-Windows | NSIS / driver tooling | Native installer chrome only; no standalone project UI theme | Platform installer behaviour local |
| Common | Library/docs | No application UI | Owns this design contract only |

The rule is simple: MB Corpo typography roles plus the shared Day/Night semantic palettes are owned by Common; System means follow the host platform. Product accents, logos, manufacturer colours, page composition and domain-specific status semantics remain local. Native C/C++ consumers should prefer `include/infiltratr/design.h`; web consumers use `infiltrator-web-v1.css`. GTK/Cinnamon/SwiftUI adapters may keep local selector/widget code, but CI should validate any generated or mirrored values against Common rather than allowing private palette truth.

SPDX-License-Identifier: GPL-3.0-or-later
