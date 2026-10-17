# Calendar resource catalogue

Common distinguishes portable compiled calendar metadata from installed executable calendar resources.

Calendars migrated to the system resource layout are deliberately absent from the compiled base catalogue. Their stable IDs enter the effective catalogue only when a validated provider module is installed under `/System/Resources/Calendars` and discovered through Common's temporal runtime APIs.

The currently externalised IDs are `hebrew`, `gregorian`, `persian`, and `coptic`. This prevents an absent module from being advertised as an available chronology and prevents compiled metadata from shadowing an installed executable provider with the same stable ID.

Common owns the provider ABI, resource discovery, validation, and merged catalogue. Individual chronology implementations remain owned by their resource modules.
