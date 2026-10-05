/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_native_completion.c
 * PURPOSE: Exercise explicit completion through the real native process transport with an independent peer.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#include "umicom/language_runtime/completion_query.h"
#include "umicom/language_runtime/completion_preview.h"

static int ProgramMain(int argc, char **argv)
{
    CHECK(argc == 3);
    const char *mode = argv[1];
    const char *known[] = {"valid",   "unicode",  "empty",    "error",  "invalid",
                           "timeout", "encoding", "shutdown", "cancel", "preview"};
    int found = 0;
    for (size_t i = 0; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    UmiLanguageServerProfile profile = {0};
    strcpy(profile.id, "native-completion-peer");
    CHECK(strlen(argv[2]) < sizeof(profile.executable));
    strcpy(profile.executable, argv[2]);
    (void)snprintf(profile.arguments, sizeof(profile.arguments), "completion-%s", mode);
    profile.enabled = 1;
    UmiLanguageCompletionRequest request = {
        "file:///workspace", "file:///workspace/main.c", "c", "pu", 2U, 2U, 3000U};
    if (strcmp(mode, "unicode") == 0)
    {
        request.source = "\xf0\x9f\x98\x80";
        request.source_bytes = 4U;
        request.cursor_offset = 4U;
    }
    if (strcmp(mode, "empty") == 0)
    {
        request.source = "";
        request.source_bytes = 0U;
        request.cursor_offset = 0U;
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
    UmiLanguageCompletionQueryReport report;
    UmiLanguageCompletionCatalogue *catalogue = NULL;
    CHECK(UmiLanguageCompletionQueryNative(&profile, NULL, &request, cancel, &report, &catalogue) ==
          expected);
    if (expected == UMI_STATUS_OK)
    {
        CHECK(report.initialized && report.document_opened && report.choices == 1U &&
              report.shutdown_status == UMI_STATUS_OK);
        if (strcmp(mode, "preview") == 0)
        {
            UmiLanguageCompletionPreview *preview = NULL;
            CHECK(UmiLanguageCompletionPreviewCreate(
                      catalogue, 0U, request.document_uri, profile.id, request.source, request.source_bytes,
                      request.cursor_offset, 0U, 2U, UMI_LANGUAGE_COMPLETION_REPLACE, NULL,
                      &preview) == UMI_STATUS_OK);
            const char *text = NULL;
            size_t size = 0U, caret = 0U;
            CHECK(UmiLanguageCompletionPreviewRead(preview, &text, &size, &caret) == UMI_STATUS_OK);
            CHECK(size == 4U && caret == 4U && strcmp(text, "puts") == 0);
            UmiLanguageCompletionPreviewDestroy(preview);
        }
    }
    else
        CHECK(catalogue == NULL);
    if (strcmp(mode, "cancel") == 0)
        CHECK(!report.started);
    UmiLanguageCompletionCatalogueDestroy(catalogue);
    umi_cancellation_token_destroy(cancel);
    return 0;
}
#include "../native_process/utf8_entry.inc"
