/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_modules/test_descriptor.c
 * PURPOSE: Exercise native module descriptors with malformed input and stable build identities.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/runtime/application_module.h"
#include "umicom/platform/filesystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define REQUIRE(condition) do { if (!(condition)) { \
    (void)fprintf(stderr, "line %d: %s\n", __LINE__, #condition); return 1; \
} } while (0)

/* Fixtures are shared with the configure checks so the two entry points read
 * the same public manifest spelling instead of separate hand-built catalogues. */
static char *load_fixture(const char *name, size_t *size)
{
    char path[UMI_PATH_CAPACITY];
    char *text = NULL;
    if (umi_fs_join(path, sizeof(path), UMI_MODULE_FIXTURES, name) != UMI_STATUS_OK ||
        umi_fs_read_text(path, &text, size) != UMI_STATUS_OK) return NULL;
    return text;
}

/* Replace one selected fragment while retaining the surrounding real manifest.
 * Allocation is exact and the caller owns the result. */
static char *replace_once(const char *text, const char *from, const char *to)
{
    const char *found = strstr(text, from);
    if (found == NULL) return NULL;
    size_t prefix = (size_t)(found - text);
    size_t suffix = strlen(found + strlen(from));
    char *result = malloc(prefix + strlen(to) + suffix + 1U);
    if (result == NULL) return NULL;
    memcpy(result, text, prefix);
    memcpy(result + prefix, to, strlen(to));
    memcpy(result + prefix + strlen(to), found + strlen(from), suffix + 1U);
    return result;
}

int main(int argc, char **argv)
{
    UmiApplicationModuleDescriptor output;
    size_t length = 0U;
    char *text;
    char *changed = NULL;
    const char *mode = argc == 2 ? argv[1] : "";
    const char *fixture = strcmp(mode, "component") == 0 ? "component.umicom.yaml" :
        strcmp(mode, "console") == 0 ? "console.umicom.yaml" : "application.umicom.yaml";
    text = load_fixture(fixture, &length);
    REQUIRE(text != NULL);
    memset(&output, 0x5a, sizeof(output));
    if (strcmp(mode, "valid") == 0 || strcmp(mode, "component") == 0 ||
        strcmp(mode, "console") == 0) {
        REQUIRE(umi_application_module_parse(text, length, &output) == UMI_STATUS_OK);
        REQUIRE(strcmp(output.id, "org.umicom.weather") == 0);
        REQUIRE(strcmp(output.option, "UMICOM_APPLICATIONS_BUILD_WEATHER") == 0);
        REQUIRE(strcmp(output.component, strcmp(mode, "component") == 0 ?
            "WeatherTools" : "UmicomWeather") == 0);
        if (strcmp(mode, "console") == 0) REQUIRE(output.native_executable[0] == '\0');
    } else if (strcmp(mode, "load") == 0) {
        char path[UMI_PATH_CAPACITY];
        REQUIRE(umi_fs_join(path, sizeof(path), UMI_MODULE_FIXTURES, fixture) == UMI_STATUS_OK);
        REQUIRE(umi_application_module_load(path, &output) == UMI_STATUS_OK);
        REQUIRE(strcmp(output.option, "UMICOM_APPLICATIONS_BUILD_WEATHER") == 0);
    } else if (strcmp(mode, "missing") == 0) {
        char path[UMI_PATH_CAPACITY];
        REQUIRE(umi_fs_join(path, sizeof(path), UMI_MODULE_FIXTURES, "absent.yaml") == UMI_STATUS_OK);
        REQUIRE(umi_application_module_load(path, &output) != UMI_STATUS_OK);
        REQUIRE(output.id[0] == '\0' && output.option[0] == '\0');
    } else if (strcmp(mode, "limit") == 0) {
        changed = malloc(UMI_APPLICATION_MODULE_MAX_BYTES + 1U);
        REQUIRE(changed != NULL);
        memset(changed, ' ', UMI_APPLICATION_MODULE_MAX_BYTES + 1U);
        memcpy(changed, text, length);
        REQUIRE(umi_application_module_parse(changed, UMI_APPLICATION_MODULE_MAX_BYTES,
            &output) == UMI_STATUS_OK);
        REQUIRE(umi_application_module_parse(changed, UMI_APPLICATION_MODULE_MAX_BYTES + 1U,
            &output) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(output.id[0] == '\0');
    } else if (strcmp(mode, "null") == 0) {
        REQUIRE(umi_application_module_parse(NULL, length, &output) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(output.option[0] == '\0');
        REQUIRE(umi_application_module_parse(text, length, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else {
        /* Failures must leave no partial identity that a wizard could display
         * as an accepted application or use for a subsequent Git operation. */
        if (strcmp(mode, "duplicate") == 0)
            changed = replace_once(text, "  id: org.umicom.weather", "  id: org.umicom.weather\n  id: org.umicom.other");
        else if (strcmp(mode, "indent") == 0)
            changed = replace_once(text, "  id:", "    id:");
        else if (strcmp(mode, "schema") == 0)
            changed = replace_once(text, "umicom.application.v1", "umicom.application/v1");
        else if (strcmp(mode, "frontend") == 0)
            changed = replace_once(text, "  - gtk\n", "");
        else if (strcmp(mode, "frontend_indent") == 0)
            changed = replace_once(text, "  - gtk", "    - gtk");
        else if (strcmp(mode, "frontend_section") == 0)
            changed = replace_once(text, "capabilities:", "frontends:\n  - console\ncapabilities:");
        else if (strcmp(mode, "version") == 0)
            changed = replace_once(text, "  version: 0.1.0", "  version: 0.1.0\n  version: 0.2.0");
        else if (strcmp(mode, "list_syntax") == 0)
            changed = replace_once(text, "Umicom Weather", "Weather;Injected");
        else if (strcmp(mode, "path") == 0)
            changed = replace_once(text, "  executable: umicom-weather", "  executable: ../weather");
        else if (strcmp(mode, "section") == 0)
            changed = replace_once(text, "\nframework:", "\napplication:\n  id: org.umicom.second\nframework:");
        else if (strcmp(mode, "embedded_nul") == 0) {
            text[length / 2U] = '\0';
            REQUIRE(umi_application_module_parse(text, length, &output) == UMI_STATUS_PARSE_ERROR);
            REQUIRE(output.option[0] == '\0');
            umi_fs_free_text(text);
            return 0;
        } else if (strcmp(mode, "crlf") == 0) {
            changed = malloc(length * 2U + 1U);
            REQUIRE(changed != NULL);
            size_t used = 0U;
            for (size_t index = 0U; index < length; ++index) {
                if (text[index] == '\n') changed[used++] = '\r';
                changed[used++] = text[index];
            }
            REQUIRE(umi_application_module_parse(changed, used, &output) == UMI_STATUS_OK);
            free(changed);
            umi_fs_free_text(text);
            return 0;
        } else REQUIRE(0 && "Unknown descriptor case");
        REQUIRE(changed != NULL);
        REQUIRE(umi_application_module_parse(changed, strlen(changed), &output) != UMI_STATUS_OK);
        const unsigned char *bytes = (const unsigned char *)&output;
        for (size_t index = 0U; index < sizeof(output); ++index) REQUIRE(bytes[index] == 0U);
    }
    free(changed);
    umi_fs_free_text(text);
    return 0;
}
