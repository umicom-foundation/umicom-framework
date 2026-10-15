/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_modules/test_descriptor_validation.c
 * PURPOSE: Validate copied application identities before native registration review.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/runtime/application_module.h"
#include <stdio.h>
#include <string.h>
#define CHECK(value) do { if (!(value)) { \
    (void)fprintf(stderr, "line %d: %s\n", __LINE__, #value); return 1; \
} } while (0)
static const char source[] =
    "schema: umicom.application.v1\napplication:\n  id: org.example.weather\n"
    "  name: Weather\n  version: 0.1.0\n  executable: umicom-weather\n"
    "  console_executable: umicom-weather\nframework:\n  minimum_version: 0.9.0\n"
    "  compatible_major: 0\nfrontends:\n  - console\ninstallation:\n  component: Weather\n";

/* Start from parser-produced metadata, then emulate edits by a UI or plugin.
 * Validation must be bounded, deterministic and leave every caller byte alone. */
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    UmiApplicationModuleDescriptor value;
    CHECK(umi_application_module_parse(source, sizeof source - 1U, &value) == UMI_STATUS_OK);
    if (strcmp(argv[1], "null") == 0) {
        CHECK(umi_application_module_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        return 0;
    }
    if (strcmp(argv[1], "unterminated") == 0) {
        char *fields[] = {value.id, value.name, value.executable, value.native_executable,
            value.console_executable, value.component, value.option};
        for (size_t i = 0U; i < 7U; ++i) {
            UmiApplicationModuleDescriptor saved = value;
            memset(fields[i], 'x', i == 6U ? sizeof value.option : sizeof value.id);
            CHECK(umi_application_module_validate(&value) == UMI_STATUS_PARSE_ERROR);
            value = saved;
        }
        return 0;
    }
    UmiStatus expected = UMI_STATUS_PARSE_ERROR;
    if (strcmp(argv[1], "valid") == 0) expected = UMI_STATUS_OK;
    else if (strcmp(argv[1], "stale_option") == 0) (void)strcpy(value.executable, "umicom-other");
    else if (strcmp(argv[1], "empty_identity") == 0) value.id[0] = '\0';
    else if (strcmp(argv[1], "path") == 0) (void)strcpy(value.console_executable, "../weather");
    else if (strcmp(argv[1], "component") == 0) (void)strcpy(value.component, "weather.test");
    else if (strcmp(argv[1], "option_case") == 0) (void)strcpy(value.option, "UMICOM_APPLICATIONS_BUILD_weather");
    else return 2;
    UmiApplicationModuleDescriptor saved = value;
    CHECK(umi_application_module_validate(&value) == expected);
    CHECK(memcmp(&value, &saved, sizeof value) == 0);
    return 0;
}
