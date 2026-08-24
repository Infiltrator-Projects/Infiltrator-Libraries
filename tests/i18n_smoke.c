// SPDX-License-Identifier: GPL-3.0-or-later
#include "infiltratr/i18n.h"

#include <assert.h>
#include <string.h>

static const InfiltratrI18nEntry english_entries[] = {
    {"nav.vehicle", "Vehicle"},
    {"scan.modules_found", "Found {count} modules"},
    {"fallback.only", "English fallback"}
};
static const InfiltratrI18nEntry german_entries[] = {
    {"nav.vehicle", "Fahrzeug"},
    {"scan.modules_found", "{count} Steuergeraete gefunden"}
};
static const InfiltratrI18nCatalog catalogs[] = {
    {"en-AU", english_entries, sizeof(english_entries) / sizeof(english_entries[0])},
    {"de-DE", german_entries, sizeof(german_entries) / sizeof(german_entries[0])}
};

int main(void)
{
    InfiltratrI18n context;
    InfiltratrI18nArgument arguments[] = {{"count", "7"}};
    char output[64];
    size_t required;

    assert(infiltratr_i18n_init(&context, catalogs,
                                sizeof(catalogs) / sizeof(catalogs[0]),
                                "en_AU"));
    assert(strcmp(infiltratr_i18n_locale(&context), "en-AU") == 0);
    assert(strcmp(infiltratr_i18n_get(&context, "nav.vehicle"), "Vehicle") == 0);

    assert(infiltratr_i18n_set_locale(&context, "de_AT.UTF-8"));
    assert(strcmp(infiltratr_i18n_locale(&context), "de-AT") == 0);
    assert(strcmp(infiltratr_i18n_get(&context, "nav.vehicle"), "Fahrzeug") == 0);
    assert(strcmp(infiltratr_i18n_get(&context, "fallback.only"),
                  "English fallback") == 0);
    assert(strcmp(infiltratr_i18n_get(&context, "missing.key"),
                  "missing.key") == 0);

    required = infiltratr_i18n_format(
        output, sizeof(output),
        infiltratr_i18n_get(&context, "scan.modules_found"),
        arguments, sizeof(arguments) / sizeof(arguments[0]));
    assert(required == strlen("7 Steuergeraete gefunden"));
    assert(strcmp(output, "7 Steuergeraete gefunden") == 0);

    required = infiltratr_i18n_format(output, 8U, "Hello {name}", NULL, 0U);
    assert(required == strlen("Hello {name}"));
    assert(strcmp(output, "Hello {") == 0);

    assert(!infiltratr_i18n_set_locale(&context, "zz-ZZ"));
    assert(strcmp(infiltratr_i18n_get(&context, "nav.vehicle"), "Vehicle") == 0);
    return 0;
}
