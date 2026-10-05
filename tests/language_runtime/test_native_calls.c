/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_native_calls.c
 * PURPOSE: Check independent call-hierarchy protocol ordering, result ownership and native cleanup boundaries.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/call_query.h"
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
    const char *known[] = {"incoming",     "outgoing",      "empty",    "no-calls",      "no-sites",
                           "multiple",     "external-root", "unicode",  "surrogate",     "outside-root",
                           "outside-site", "invalid-root",  "invalid",  "prepare-error", "error",
                           "second-error", "shutdown",      "encoding", "timeout",       "disconnect"};
    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    UmiLanguageServerProfile profile = {0};
    strcpy(profile.id, "native-calls");
    profile.enabled = 1;
    CHECK(strlen(argv[2]) < sizeof(profile.executable));
    strcpy(profile.executable, argv[2]);
    (void)snprintf(profile.arguments, sizeof(profile.arguments), "calls-%s", mode);
    UmiLanguageCallRequest request = {
        {"file:///workspace", "file:///workspace/main.c", "c", "pu", 2U, 2U, 2000U},
        UMI_LANGUAGE_CALL_INCOMING};
    if (strcmp(mode, "outgoing") == 0 || strcmp(mode, "external-root") == 0)
        request.direction = UMI_LANGUAGE_CALL_OUTGOING;
    if (strcmp(mode, "unicode") == 0 || strcmp(mode, "surrogate") == 0)
    {
        request.source.source = "a\xf0\x9f\x98\x80"
                                "b";
        request.source.source_bytes = 6U;
        request.source.cursor_offset = 5U;
    }
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(mode, "surrogate") == 0 || strcmp(mode, "outside-root") == 0 ||
        strcmp(mode, "outside-site") == 0)
        expected = UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(mode, "invalid-root") == 0 || strcmp(mode, "invalid") == 0)
        expected = UMI_STATUS_PARSE_ERROR;
    if (strcmp(mode, "prepare-error") == 0 || strcmp(mode, "error") == 0 ||
        strcmp(mode, "second-error") == 0 || strcmp(mode, "shutdown") == 0)
        expected = UMI_STATUS_UNAVAILABLE;
    if (strcmp(mode, "encoding") == 0)
        expected = UMI_STATUS_NOT_IMPLEMENTED;
    if (strcmp(mode, "timeout") == 0)
    {
        request.source.timeout_ms = 300U;
        expected = UMI_STATUS_TIMEOUT;
    }
    UmiLanguageCompletionQueryReport report;
    UmiLanguageCallResult *result = NULL;
    UmiStatus status = UmiLanguageCallQueryNative(&profile, NULL, &request, NULL, &report, &result);
    CHECK(strcmp(mode, "disconnect") == 0 ? status != UMI_STATUS_OK : status == expected);
    CHECK(report.started);
    if (status == UMI_STATUS_OK)
    {
        size_t roots = strcmp(mode, "empty") == 0 ? 0U : strcmp(mode, "multiple") == 0 ? 2U : 1U;
        size_t count = strcmp(mode, "no-calls") == 0 ? 0U : roots;
        CHECK(result != NULL && UmiLanguageCallResultCount(result) == count &&
              UmiLanguageCallResultRootCount(result) == roots && report.choices == count);
        CHECK(report.initialized && report.document_opened && report.close_status == UMI_STATUS_OK &&
              report.shutdown_status == UMI_STATUS_OK);
        for (size_t i = 0U; i < count; ++i)
        {
            UmiLanguageCallRelation relation;
            CHECK(UmiLanguageCallResultAt(result, i, &relation) == UMI_STATUS_OK && relation.root_index == i);
            CHECK(relation.call_sites == (strcmp(mode, "no-sites") == 0 ? 0U : 2U));
            if (relation.call_sites != 0U)
            {
                UmiLanguageSourceLocation location;
                CHECK(UmiLanguageCallResultLocation(result, i, 2U, &location) == UMI_STATUS_OK);
                CHECK(strcmp(location.uri, strcmp(mode, "external-root") == 0
                                               ? "file:///workspace/definition.c"
                                               : request.source.document_uri) == 0);
            }
        }
    }
    else
        CHECK(result == NULL);
    UmiLanguageCallResultDestroy(result);
    return 0;
}
#include "../native_process/utf8_entry.inc"
