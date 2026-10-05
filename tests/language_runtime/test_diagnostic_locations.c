/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_diagnostic_locations.c
 * PURPOSE: Check complete diagnostic destinations, borrowed ownership and unchanged error outputs.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/diagnostic_catalogue.h"
#include <stdio.h>
#include <stdlib.h>
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
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *known[] = {"primary",     "first-related", "second-related",     "second-diagnostic",
                           "no-related",  "empty",         "invalid-diagnostic", "invalid-location",
                           "large-index", "null-owner",    "null-output",        "owned-uri",
                           "pull"};
    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    const char *items = "[{\"message\":\"First\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
                        "\"line\":0,\"character\":2}},\"relatedInformation\":[{\"message\":\"Same "
                        "source\",\"location\":{\"uri\":\"file:///"
                        "main.c\",\"range\":{\"start\":{\"line\":1,\"character\":1},\"end\":{\"line\":2,"
                        "\"character\":2}}}},{\"message\":\"Other "
                        "source\",\"location\":{\"uri\":\"file:///"
                        "related.c\",\"range\":{\"start\":{\"line\":3,\"character\":4},\"end\":{\"line\":5,"
                        "\"character\":6}}}}]},{\"message\":\"Second\",\"range\":{\"start\":{\"line\":7,"
                        "\"character\":8},\"end\":{\"line\":9,\"character\":10}}}]";
    if (strcmp(mode, "empty") == 0)
        items = "[]";
    char json[4096];
    int written = snprintf(json, sizeof(json), "{\"uri\":\"file:///main.c\",\"diagnostics\":%s}", items);
    CHECK(written > 0 && (size_t)written < sizeof(json));
    UmiLanguageDiagnosticCatalogue *catalogue = NULL;
    if (strcmp(mode, "pull") == 0)
    {
        written =
            snprintf(json, sizeof(json),
                     "{\"jsonrpc\":\"2.0\",\"id\":7,\"result\":{\"kind\":\"full\",\"items\":%s}}", items);
        CHECK(written > 0 && (size_t)written < sizeof(json));
        CHECK(UmiLanguageDiagnosticCatalogueReadPullResponse(json, (size_t)written, 7U, "file:///main.c",
                                                             NULL, &catalogue) == UMI_STATUS_OK);
    }
    else
        CHECK(UmiLanguageDiagnosticCatalogueCreate(json, (size_t)written, NULL, &catalogue) == UMI_STATUS_OK);
    if (strcmp(mode, "owned-uri") == 0)
        memset(json, 'x', (size_t)written);
    size_t diagnostic = 0U, location = 0U;
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(mode, "first-related") == 0 || strcmp(mode, "owned-uri") == 0)
        location = 1U;
    if (strcmp(mode, "second-related") == 0 || strcmp(mode, "pull") == 0)
        location = 2U;
    if (strcmp(mode, "second-diagnostic") == 0 || strcmp(mode, "no-related") == 0)
        diagnostic = 1U;
    if (strcmp(mode, "no-related") == 0)
    {
        location = 1U;
        expected = UMI_STATUS_NOT_FOUND;
    }
    if (strcmp(mode, "invalid-diagnostic") == 0)
    {
        diagnostic = 2U;
        expected = UMI_STATUS_NOT_FOUND;
    }
    if (strcmp(mode, "invalid-location") == 0)
    {
        location = 3U;
        expected = UMI_STATUS_NOT_FOUND;
    }
    if (strcmp(mode, "large-index") == 0)
    {
        location = SIZE_MAX;
        expected = UMI_STATUS_NOT_FOUND;
    }
    if (strcmp(mode, "empty") == 0)
        expected = UMI_STATUS_NOT_FOUND;
    if (strcmp(mode, "null-owner") == 0 || strcmp(mode, "null-output") == 0)
        expected = UMI_STATUS_INVALID_ARGUMENT;
    UmiLanguageSourceLocation output = {.uri = "sentinel", .target = {{91U, 92U}, {93U, 94U}}, .is_link = 1};
    CHECK(UmiLanguageDiagnosticCatalogueLocation(
              strcmp(mode, "null-owner") == 0 ? NULL : catalogue, diagnostic, location,
              strcmp(mode, "null-output") == 0 ? NULL : &output) == expected);
    if (expected == UMI_STATUS_OK)
    {
        CHECK(!output.is_link && !output.has_origin);
        CHECK(strcmp(output.uri, location == 2U ? "file:///related.c" : "file:///main.c") == 0);
        CHECK(output.target.start.line == (diagnostic == 1U ? 7U
                                           : location == 2U ? 3U
                                           : location == 1U ? 1U
                                                            : 0U));
        CHECK(output.target.start.utf16_column == (diagnostic == 1U ? 8U
                                                   : location == 2U ? 4U
                                                   : location == 1U ? 1U
                                                                    : 0U));
        CHECK(output.target.end.line == (diagnostic == 1U ? 9U
                                         : location == 2U ? 5U
                                         : location == 1U ? 2U
                                                          : 0U));
        CHECK(output.target.end.utf16_column == (diagnostic == 1U ? 10U : location == 2U ? 6U : 2U));
        CHECK(output.selection.start.line == output.target.start.line &&
              output.selection.start.utf16_column == output.target.start.utf16_column);
        CHECK(output.selection.end.line == output.target.end.line &&
              output.selection.end.utf16_column == output.target.end.utf16_column);
    }
    else
        CHECK(strcmp(output.uri, "sentinel") == 0 && output.target.start.line == 91U && output.is_link == 1);
    UmiLanguageDiagnosticCatalogueDestroy(catalogue);
    return 0;
}
