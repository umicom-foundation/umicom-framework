/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_native_folding_ranges.c
 * PURPOSE: Check native folding discovery against an independently implemented stdio peer.
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
    const char *known[] = {"valid",   "empty",   "empty-array", "single",   "unicode", "crlf",
                           "outside", "invalid", "error",       "shutdown", "encoding"};
    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    UmiLanguageServerProfile profile = {0};
    strcpy(profile.id, "native-folding");
    profile.enabled = 1;
    CHECK(strlen(argv[2]) < sizeof(profile.executable));
    strcpy(profile.executable, argv[2]);
    int written = snprintf(profile.arguments, sizeof(profile.arguments), "foldings-%s", mode);
    CHECK(written > 0 && (size_t)written < sizeof(profile.arguments));
    const char *source = strcmp(mode, "unicode") == 0 ? "h\n\xf0\x9f\x98\x80\nend\ntail"
                         : strcmp(mode, "crlf") == 0  ? "head\r\nbody\r\nend\r\ntail"
                                                      : "head\nbody\nend\ntail";
    if (strcmp(mode, "empty") == 0)
        source = "";
    UmiLanguageNavigationRequest request = {"file:///workspace",
                                            "file:///workspace/main.c",
                                            "c",
                                            source,
                                            strlen(source),
                                            0U,
                                            3000U,
                                            UMI_LANGUAGE_NAVIGATION_FOLDING_RANGES,
                                            0};
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(mode, "outside") == 0)
        expected = UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(mode, "invalid") == 0)
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
        size_t count = strcmp(mode, "empty") == 0 || strcmp(mode, "empty-array") == 0 ? 0U
                       : strcmp(mode, "single") == 0                                  ? 1U
                                                                                      : 2U;
        CHECK(catalogue != NULL && report.locations == count &&
              UmiLanguageLocationCatalogueCount(catalogue) == count);
        CHECK(report.initialized && report.document_opened && report.close_status == UMI_STATUS_OK &&
              report.shutdown_status == UMI_STATUS_OK);
        if (count != 0U)
        {
            UmiLanguageSourceLocation item;
            CHECK(UmiLanguageLocationCatalogueAt(catalogue, 0U, &item) == UMI_STATUS_OK);
            CHECK(strcmp(item.uri, request.document_uri) == 0 &&
                  item.selection.end.line == (count == 1U ? 2U : 3U) &&
                  item.selection.end.utf16_column == 0U);
        }
    }
    else
        CHECK(catalogue == NULL);
    UmiLanguageLocationCatalogueDestroy(catalogue);
    return 0;
}
#include "../native_process/utf8_entry.inc"
