# Infiltrator Design consumers

This ledger tracks graphical consumers of `infiltrator-design-v1.json`.

`InfiltratorFS` is intentionally excluded from this migration while its active structural/UI work is in progress.

| Repository | UI surface | Typography state | Design ownership |
| --- | --- | --- | --- |
| MBLINK | GTK / SwiftUI | Reference implementation | Product accent/identity local; shared structure through LINK |
| Calendar-Plus | Cinnamon / GTK | MB Corpo roles implemented with fallback | Calendar identity local |
| Defragger | GTK / Cairo | MB Corpo provisioned/used; residual Cairo font alignment pending | Defragger identity local |
| RunnerScope | Tk | MB Corpo roles declared with fallback/private local discovery | RunnerScope identity local |
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
| Infiltrator-Repository | Web | Uses system-ui/monospace stacks; migration pending | Repository/site identity local |
| ExtFS-for-Windows | NSIS / driver tooling | Native installer chrome only; no standalone project UI theme | Platform installer behaviour local |
| Infiltrator-Libraries | Library/docs | No application UI | Owns this design contract only |

The rule is simple: MB Corpo typography and the graphite/silver structural language are shared; product accents, logos, manufacturer colours, and domain-specific status semantics remain local.

SPDX-License-Identifier: GPL-3.0-or-later
