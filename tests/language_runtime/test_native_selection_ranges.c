/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_native_selection_ranges.c
 * PURPOSE: Exercise independently checked native selection requests and complete cleanup-gated ranges.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/navigation_query.h"
#include <stdio.h>
#include <string.h>
#define CHECK(c)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(c))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%d: %s\n", __LINE__, #c);                                                       \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
static int ProgramMain(int argc, char **argv)
{
    CHECK(argc == 3);
    const char *mode = argv[1];
    const char *known[] = {"valid",     "start",    "empty",   "unicode", "multiline",
                           "surrogate", "outside",  "shrinks", "invalid", "empty-array",
                           "error",     "shutdown", "encoding"};
    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    UmiLanguageServerProfile profile = {0};
    strcpy(profile.id, "native-selection-ranges");
    profile.enabled = 1;
    CHECK(strlen(argv[2]) < sizeof(profile.executable));
    strcpy(profile.executable, argv[2]);
    int written = snprintf(profile.arguments, sizeof(profile.arguments), "selections-%s", mode);
    CHECK(written > 0 && (size_t)written < sizeof(profile.arguments));
    UmiLanguageNavigationRequest request = {"file:///workspace",
                                            "file:///workspace/main.c",
                                            "c",
                                            "pu",
                                            2U,
                                            2U,
                                            3000U,
                                            UMI_LANGUAGE_NAVIGATION_SELECTION_RANGES,
                                            0};
    if (strcmp(mode, "start") == 0)
        request.caret = 0U;
    if (strcmp(mode, "empty") == 0)
    {
        request.source = "";
        request.source_bytes = 0U;
        request.caret = 0U;
    }
    if (strcmp(mode, "unicode") == 0 || strcmp(mode, "surrogate") == 0)
    {
        request.source = "a\xf0\x9f\x98\x80"
                         "b";
        request.source_bytes = 6U;
        request.caret = strcmp(mode, "unicode") == 0 ? 5U : 1U;
    }
    if (strcmp(mode, "multiline") == 0)
    {
        request.source = "a\nbc";
        request.source_bytes = 4U;
        request.caret = 2U;
    }
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(mode, "surrogate") == 0 || strcmp(mode, "outside") == 0)
        expected = UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(mode, "shrinks") == 0 || strcmp(mode, "invalid") == 0 || strcmp(mode, "empty-array") == 0)
        expected = UMI_STATUS_PARSE_ERROR;
    if (strcmp(mode, "error") == 0 || strcmp(mode, "shutdown") == 0)
        expected = UMI_STATUS_UNAVAILABLE;
    if (strcmp(mode, "encoding") == 0)
        expected = UMI_STATUS_NOT_IMPLEMENTED;
    UmiLanguageNavigationReport report;
    UmiLanguageLocationCatalogue *catalogue = NULL;
    CHECK(UmiLanguageNavigationQueryNative(&profile, NULL, &request, NULL, &report, &catalogue) == expected);
    CHECK(report.started);
    if (expected == UMI_STATUS_OK)
    {
        size_t count = strcmp(mode, "empty") == 0 ? 0U : 2U;
        CHECK(catalogue != NULL && report.locations == count &&
              UmiLanguageLocationCatalogueCount(catalogue) == count);
        CHECK(report.initialized && report.document_opened && report.close_status == UMI_STATUS_OK &&
              report.shutdown_status == UMI_STATUS_OK);
        if (count != 0U)
        {
            UmiLanguageSourceLocation item;
            CHECK(UmiLanguageLocationCatalogueAt(catalogue, 1U, &item) == UMI_STATUS_OK);
            CHECK(strcmp(item.uri, request.document_uri) == 0 && item.selection.start.line == 0U &&
                  item.selection.start.utf16_column == 0U);
        }
    }
    else
        CHECK(catalogue == NULL);
    UmiLanguageLocationCatalogueDestroy(catalogue);
    return 0;
}
#include "../native_process/utf8_entry.inc"
