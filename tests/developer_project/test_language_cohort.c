/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_language_cohort.c
 *
 * PURPOSE:
 *   Verify the report order, discovery scope and incomplete-output guard.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/language_cohort.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/path.h"

#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "Language cohort check failed at line %d: %s\n", __LINE__, #condition); \
        return 1; \
    } \
} while (0)

int main(void)
{
    char output[UMI_PROJECT_LANGUAGE_COHORT_JSON_CAPACITY];
    const char *const ids[] = {
        "c23", "cpp", "umicom-c", "assembly", "rust", "go", "zig", "d",
        "freebasic", "pascal", "fortran", "ada", "swift", "java", "csharp",
        "kotlin", "typescript", "cpython", "javascript", "php"
    };
    char directory[UMI_PATH_CAPACITY];
    CHECK(umi_fs_current_directory(directory, sizeof directory) == UMI_STATUS_OK);
    CHECK(UmiProjectLanguageCohortFormatJson(directory, output, sizeof output) == UMI_STATUS_OK);
    CHECK(strncmp(output, "{\"schema\":1,", strlen("{\"schema\":1,")) == 0);
    CHECK(strstr(output, "\"evidence\":\"tool-file-lookup\"") != NULL);
    CHECK(strstr(output, "\"tool_selection\":\"explicit-directory\"") != NULL);
    const char *row = output;
    const char *const id_prefix = "\"id\":\"developer.language.";
    for (size_t index = 0U; index < sizeof ids / sizeof ids[0]; ++index) {
        row = strstr(row, id_prefix);
        CHECK(row != NULL);
        row += strlen(id_prefix);
        size_t length = strlen(ids[index]);
        CHECK(strncmp(row, ids[index], length) == 0 && row[length] == '"');
        const char *compiled = strstr(row, "\"compiled\":");
        CHECK(compiled != NULL);
        CHECK(strncmp(compiled, index < 17U ? "\"compiled\":true" : "\"compiled\":false",
                      strlen(index < 17U ? "\"compiled\":true" : "\"compiled\":false")) == 0);
    }
    CHECK(strstr(row, id_prefix) == NULL);
    CHECK(strlen(output) > 2U && strcmp(output + strlen(output) - 2U, "]}") == 0);
    CHECK(UmiProjectLanguageCohortFormatJson(directory, output, 20U) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(output[0] == '\0');
    output[0] = 'x';
    CHECK(UmiProjectLanguageCohortFormatJson("relative", output, sizeof output) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(output[0] == '\0');
    CHECK(UmiProjectLanguageCohortFormatJson(directory, NULL, sizeof output) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
