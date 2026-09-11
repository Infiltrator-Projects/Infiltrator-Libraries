# Infiltrator Design consumers

This ledger tracks graphical consumers of `infiltrator-design-v1.json`.

`InfiltratorFS` is intentionally excluded from this migration while its active structural/UI work is in progress.

| Repository | UI surface | Typography state | Design ownership |
| --- | --- | --- | --- |
| MBLINK | GTK / SwiftUI | Reference implementation | Product accent/identity local; shared structure through LINK |
| Calendar-Plus | Cinnamon / GTK | MB Corpo roles already implemented with fallback | Calendar identity local |
| Defragger | GTK / Cairo | MB Corpo already provisioned/used; alignment pass required for residual `Sans` drawing | Defragger identity local |
| RunnerScope | Tk | MB Corpo roles already declared with fallback | RunnerScope identity local |
| LINK | GTK / Win32 / SwiftUI | Shared LINK shells; typography convergence target | Shared vehicle application shell |
| JAGLINK | LINK + SwiftUI | LINK geometry; product theme still contains system-font overrides | Jaguar identity local |
| BMWLINK | LINK + SwiftUI | LINK geometry; product theme still contains system-font overrides | BMW identity local |
| AUDILINK | LINK + SwiftUI | LINK geometry; product theme still contains system-font overrides | Audi identity local |
| FORDLINK | LINK + SwiftUI | LINK geometry; product theme still contains system-font overrides | Ford identity local |
| System-Monitor | GTK | Uses GTK/system typography; migration target | System Monitor identity local |
| Character-Profiler | SwiftUI | Uses system typography; migration target | Character Profiler accent/identity local |
| Keeper-Group-Export | Tk/ttk | Uses Segoe UI; migration target | Keeper accent/identity local |
| WheresWally | Zabbix web widget | Inherits Zabbix/monospace fonts; migration target | NPS operational colour semantics local |
| Intune-Zabbix-Bridge | Zabbix web widget | Inherits Zabbix/monospace fonts; migration target | Intune operational colour semantics local |
| Infiltrator-Repository | Web | Uses system-ui/monospace stacks; migration target | Repository/site identity local |
| ExtFS-for-Windows | NSIS / driver tooling | Native installer chrome only; no standalone project UI theme | Platform installer behaviour local |
| Infiltrator-Repository | Web | Shared Infiltrator web presentation target | Repository/site identity local |
| Infiltrator-Libraries | Library/docs | No application UI | Owns this design contract only |

The rule is simple: MB Corpo typography and the graphite/silver structural language are shared; product accents, logos, manufacturer colours, and domain-specific status semantics remain local.

SPDX-License-Identifier: GPL-3.0-or-later
