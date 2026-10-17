/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_instructions/test_response_fields.c
 * PURPOSE: Check adapter field types at the debugger response schema boundary.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "../../src/debug_runtime/response_fields_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(value) do { if (!(value)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); result = 1; goto done; } } while (0)

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    struct FieldCase { const char *name; const char *json; int numeric; UmiStatus expected; };
    const struct FieldCase cases[] = {
        {"boolean-string", "{\"value\":\"yes\"}", 0, UMI_STATUS_PARSE_ERROR},
        {"boolean-object", "{\"value\":{}}", 0, UMI_STATUS_PARSE_ERROR},
        {"boolean-array", "{\"value\":[]}", 0, UMI_STATUS_PARSE_ERROR},
        {"boolean-null", "{\"value\":null}", 0, UMI_STATUS_PARSE_ERROR},
        {"boolean-number", "{\"value\":1}", 0, UMI_STATUS_PARSE_ERROR},
        {"boolean-true", "{\"value\":true}", 0, UMI_STATUS_OK},
        {"boolean-false", "{\"value\":false}", 0, UMI_STATUS_OK},
        {"unsigned-string", "{\"value\":\"1\"}", 1, UMI_STATUS_PARSE_ERROR},
        {"unsigned-object", "{\"value\":{}}", 1, UMI_STATUS_PARSE_ERROR},
        {"unsigned-array", "{\"value\":[]}", 1, UMI_STATUS_PARSE_ERROR},
        {"unsigned-null", "{\"value\":null}", 1, UMI_STATUS_PARSE_ERROR},
        {"unsigned-boolean", "{\"value\":true}", 1, UMI_STATUS_PARSE_ERROR},
        {"unsigned-fraction", "{\"value\":1.5}", 1, UMI_STATUS_PARSE_ERROR},
        {"unsigned-negative", "{\"value\":-1}", 1, UMI_STATUS_PARSE_ERROR},
        {"unsigned-bound", "{\"value\":11}", 1, UMI_STATUS_PARSE_ERROR},
        {"unsigned-valid", "{\"value\":10}", 1, UMI_STATUS_OK}
    };
    const struct FieldCase *selected = NULL;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
        if (strcmp(argv[1], cases[i].name) == 0) selected = &cases[i];
    if (selected == NULL) return 2;
    /* The bounded JSON token arena belongs on the heap, including on Windows
     * where these small native test processes use the ordinary stack limit. */
    UmiLanguageRuntimeJsonDocument *document = malloc(sizeof *document);
    if (document == NULL) return 1;
    int result = 0;
    CHECK(umi_language_runtime_json_parse(selected->json, document) == UMI_STATUS_OK);
    CHECK(DebugResponseJsonObject(document, 0) == UMI_STATUS_OK);
    if (selected->numeric)
    {
        uint64_t value = 999U;
        int known = 0;
        CHECK(DebugResponseJsonUnsigned(document, 0, "value", 1, 10U, &value, &known) == selected->expected);
        CHECK(known && value == (selected->expected == UMI_STATUS_OK ? 10U : 0U));
    }
    else
    {
        int value = 99;
        CHECK(DebugResponseJsonBoolean(document, 0, "value", 1, &value) == selected->expected);
        CHECK(value == (selected->expected != UMI_STATUS_OK ? 99 : strcmp(argv[1], "boolean-true") == 0));
    }
    /* Missing optional fields retain their documented defaults; malformed
     * present fields must not be mistaken for omitted data. */
    int missing = 99;
    CHECK(DebugResponseJsonBoolean(document, 0, "missing", 0, &missing) == UMI_STATUS_OK && missing == 0);
    CHECK(DebugResponseJsonBoolean(document, 0, "missing", 1, &missing) == UMI_STATUS_PARSE_ERROR);
    uint64_t absent = 999U;
    int known = 1;
    CHECK(DebugResponseJsonUnsigned(document, 0, "missing", 0, 10U, &absent, &known) == UMI_STATUS_OK);
    CHECK(!known && absent == 0U);
done:
    free(document);
    return result;
}
