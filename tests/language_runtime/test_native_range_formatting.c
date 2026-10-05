/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_native_range_formatting.c
 * PURPOSE: Check range formatting through a native child and an independent protocol peer.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#include "umicom/language_runtime/formatting_query.h"
static int ProgramMain(int argc, char **argv)
{
    CHECK(argc == 3);
    const char *mode = argv[1];
    const char *known[] = {"valid",   "unicode",  "empty",    "tabs",   "error",       "invalid",
                           "timeout", "encoding", "shutdown", "cancel", "surrounding", "multiline"};
    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    UmiLanguageServerProfile profile = {0};
    strcpy(profile.id, "native-range-formatting-peer");
    CHECK(strlen(argv[2]) < sizeof(profile.executable));
    strcpy(profile.executable, argv[2]);
    profile.enabled = 1;
    (void)snprintf(profile.arguments, sizeof(profile.arguments), "range-formatting-%s", mode);
    UmiLanguageFormattingRequest request = {
        "file:///workspace", "file:///workspace/main.c", "c", "pu", 2U, 2U, 3000U, 4U, 1};
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
    if (strcmp(mode, "tabs") == 0)
    {
        request.tab_size = 2U;
        request.insert_spaces = 0;
    }
    if (strcmp(mode, "multiline") == 0)
    {
        request.source = "pu\n";
        request.source_bytes = request.caret = 3U;
    }
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(mode, "error") == 0 || strcmp(mode, "shutdown") == 0)
        expected = UMI_STATUS_UNAVAILABLE;
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
    UmiLanguageRangeFormattingRequest selected = {request, strcmp(mode, "surrounding") == 0 ? 1U : 0U,
                                                  request.source_bytes};
    UmiLanguageFormattingReport report;
    UmiLanguageTextEditPreview *preview = NULL;
    CHECK(UmiLanguageRangeFormattingQueryNative(&profile, NULL, &selected, cancel, &report, &preview) ==
          expected);
    if (expected == UMI_STATUS_OK)
    {
        const char *text = NULL;
        size_t bytes = 0U, caret = 0U;
        CHECK(report.initialized && report.document_opened && report.edit_count == 1U &&
              report.shutdown_status == UMI_STATUS_OK);
        CHECK(UmiLanguageTextEditPreviewRead(preview, &text, &bytes, &caret) == UMI_STATUS_OK);
        CHECK(bytes == (strcmp(mode, "multiline") == 0 ? 5U : 4U) && caret == bytes &&
              strcmp(text, strcmp(mode, "multiline") == 0 ? "puts\n" : "puts") == 0);
    }
    else
        CHECK(preview == NULL);
    if (strcmp(mode, "cancel") == 0)
        CHECK(!report.started);
    UmiLanguageTextEditPreviewDestroy(preview);
    umi_cancellation_token_destroy(cancel);
    return 0;
}
#include "../native_process/utf8_entry.inc"
