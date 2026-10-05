/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_native_type_navigation.c
 * PURPOSE: Check type definitions and implementations through a native child and an independent protocol peer.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#include "umicom/language_runtime/navigation_query.h"
static int ProgramMain(int argc, char **argv)
{
    CHECK(argc == 3);
    const char *mode = argv[1];
    const char *known[] = {"type-valid",
                           "type-unicode",
                           "type-empty",
                           "type-link",
                           "type-error",
                           "type-invalid",
                           "type-timeout",
                           "type-encoding",
                           "type-shutdown",
                           "type-cancel",
                           "type-range",
                           "implementation-valid",
                           "implementation-unicode",
                           "implementation-empty",
                           "implementation-link",
                           "implementation-error",
                           "implementation-invalid",
                           "implementation-timeout",
                           "implementation-encoding",
                           "implementation-shutdown",
                           "implementation-cancel",
                           "implementation-range"};
    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    int implementation = strncmp(mode, "implementation-", 15U) == 0;
    mode += implementation ? 15U : 5U;
    UmiLanguageServerProfile profile = {0};
    strcpy(profile.id, "native-navigation-peer");
    CHECK(strlen(argv[2]) < sizeof(profile.executable));
    strcpy(profile.executable, argv[2]);
    profile.enabled = 1;
    (void)snprintf(profile.arguments, sizeof(profile.arguments), "%s-navigation-%s",
                   implementation ? "implementation" : "type", mode);
    UmiLanguageNavigationRequest request = {"file:///workspace",
                                            "file:///workspace/main.c",
                                            "c",
                                            "pu",
                                            2U,
                                            2U,
                                            3000U,
                                            implementation ? UMI_LANGUAGE_NAVIGATION_IMPLEMENTATION
                                                           : UMI_LANGUAGE_NAVIGATION_TYPE_DEFINITION,
                                            0};
    if (strcmp(mode, "unicode") == 0)
    {
        request.source = "\xf0\x9f\x98\x80";
        request.source_bytes = request.caret = 4U;
    }
    if (strcmp(mode, "empty") == 0)
    {
        request.source = "";
        request.source_bytes = request.caret = 0U;
    }
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(mode, "error") == 0 || strcmp(mode, "shutdown") == 0)
        expected = UMI_STATUS_UNAVAILABLE;
    if (strcmp(mode, "range") == 0)
        expected = UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(mode, "invalid") == 0)
        expected = UMI_STATUS_PARSE_ERROR;
    if (strcmp(mode, "encoding") == 0)
        expected = UMI_STATUS_NOT_IMPLEMENTED;
    if (strcmp(mode, "timeout") == 0)
    {
        expected = UMI_STATUS_TIMEOUT;
        request.timeout_ms = 100U;
    }
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    if (strcmp(mode, "cancel") == 0)
    {
        umi_cancellation_token_request(cancel);
        expected = UMI_STATUS_CANCELLED;
    }
    UmiLanguageNavigationReport report;
    UmiLanguageLocationCatalogue *preview = NULL;
    CHECK(UmiLanguageNavigationQueryNative(&profile, NULL, &request, cancel, &report, &preview) == expected);
    if (expected == UMI_STATUS_OK)
    {
        size_t count = strcmp(mode, "empty") == 0 ? 0U : 1U;
        CHECK(report.initialized && report.document_opened && report.locations == count &&
              report.shutdown_status == UMI_STATUS_OK);
        CHECK(UmiLanguageLocationCatalogueCount(preview) == count);
        if (count != 0U)
        {
            UmiLanguageSourceLocation location;
            CHECK(UmiLanguageLocationCatalogueAt(preview, 0U, &location) == UMI_STATUS_OK);
            int link = strcmp(mode, "link") == 0;
            CHECK(location.is_link == link && location.has_origin == link);
            CHECK(strcmp(location.uri, link ? "file:///workspace/other.c" : "file:///workspace/main.c") == 0);
            CHECK(location.selection.start.utf16_column == (link ? 1U : 0U));
        }
    }
    else
        CHECK(preview == NULL);
    if (strcmp(mode, "cancel") == 0)
        CHECK(!report.started);
    UmiLanguageLocationCatalogueDestroy(preview);
    umi_cancellation_token_destroy(cancel);
    return 0;
}
#include "../native_process/utf8_entry.inc"
