/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_version_consistency.c
 *
 * PURPOSE:
 *   Prevent the public Framework version, CMake project version, release
 *   heading, current application templates, and SDK locator from drifting.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/base/version.h"

#ifndef UMICOM_FRAMEWORK_PROJECT_VERSION
#error "UMICOM_FRAMEWORK_PROJECT_VERSION must be supplied by CMake"
#endif

#ifndef UMICOM_FRAMEWORK_SOURCE_DIR
#error "UMICOM_FRAMEWORK_SOURCE_DIR must identify the Framework source root"
#endif

static int file_contains(const char *relative_path, const char *expected)
{
    char path[1024];
    char line[4096];
    FILE *stream;
    const int written = snprintf(path, sizeof(path), "%s/%s",
                                 UMICOM_FRAMEWORK_SOURCE_DIR, relative_path);
    if (written < 0 || (size_t)written >= sizeof(path)) return 0;
    stream = fopen(path, "r");
    if (stream == NULL) return 0;
    while (fgets(line, (int)sizeof(line), stream) != NULL) {
        if (strstr(line, expected) != NULL) {
            (void)fclose(stream);
            return 1;
        }
    }
    (void)fclose(stream);
    return 0;
}

int main(void)
{
    assert(strcmp(UMICOM_FRAMEWORK_VERSION_STRING,
                  UMICOM_FRAMEWORK_PROJECT_VERSION) == 0);
    assert(UMICOM_FRAMEWORK_VERSION_MAJOR == 0);
    assert(UMICOM_FRAMEWORK_VERSION_MINOR == 9);
    assert(UMICOM_FRAMEWORK_VERSION_PATCH == 0);
    assert(file_contains("README.md", "## Major foundation 0.9.0"));
    assert(file_contains(
        "templates/repository/applications/@APP_SLUG@/application.umicom.yaml.in",
        "minimum_version: 0.9.0"));
    assert(file_contains(
        "templates/application/application.umicom.yaml.in",
        "minimum_version: 0.9.0"));
    assert(file_contains("src/sdk/locator.c",
                         "UMICOM_FRAMEWORK_VERSION_STRING"));
    return 0;
}
