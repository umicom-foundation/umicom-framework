/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_native_code_actions.c
 * PURPOSE: Exercise complete code actions through a native peer without executing returned commands.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#include "umicom/language_runtime/code_action_query.h"
static int ProgramMain(int argc, char **argv)
{
    CHECK(argc == 3);
    const char *mode = argv[1];
    const char *known[] = {"valid",    "unicode",    "multiline",    "empty",    "error",
                           "invalid",  "timeout",    "encoding",     "shutdown", "cancel",
                           "disabled", "command",    "edit-command", "deferred", "external",
                           "resource", "annotation", "choices",      "literal"};

    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    UmiLanguageServerProfile profile = {0};
    strcpy(profile.id, "native-actions-peer");
    CHECK(strlen(argv[2]) < sizeof(profile.executable));
    strcpy(profile.executable, argv[2]);
    profile.enabled = 1;
    (void)snprintf(profile.arguments, sizeof(profile.arguments), "actions-%s", mode);
    UmiLanguageCodeActionRequest request = {
        "file:///workspace", "file:///workspace/main.c", "c", "abc", 3U, 0U, 3U, 3000U};
    if (strcmp(mode, "unicode") == 0)
    {
        request.source = "\xf0\x9f\x98\x80";
        request.source_bytes = request.range_end = 4U;
    }
    if (strcmp(mode, "multiline") == 0)
    {
        request.source = "abc\r\nx";
        request.source_bytes = request.range_end = 6U;
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
        request.timeout_ms = 100U;
        expected = UMI_STATUS_TIMEOUT;
    }
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    if (strcmp(mode, "cancel") == 0)
    {
        umi_cancellation_token_request(cancel);
        expected = UMI_STATUS_CANCELLED;
    }
    UmiLanguageCodeActionReport report;
    UmiLanguageCodeActionCatalogue *catalogue = NULL;
    UmiStatus status =
        UmiLanguageCodeActionQueryNative(&profile, NULL, &request, cancel, &report, &catalogue);
    if (status != expected)
        fprintf(stderr, "%s: status %d expected %d\n", mode, (int)status, (int)expected);
    CHECK(status == expected);
    if (status == UMI_STATUS_OK)
    {
        size_t count = strcmp(mode, "empty") == 0 ? 0U : strcmp(mode, "choices") == 0 ? 2U : 1U;
        CHECK(report.initialized && report.document_opened && report.actions == count &&
              report.close_status == UMI_STATUS_OK && report.shutdown_status == UMI_STATUS_OK);
        CHECK(UmiLanguageCodeActionCatalogueCount(catalogue) == count);
        if (count != 0U)
        {
            UmiLanguageCodeAction action;
            CHECK(UmiLanguageCodeActionCatalogueAt(catalogue, 0U, &action) == UMI_STATUS_OK);
            if (strcmp(mode, "literal") == 0)
                CHECK(strcmp(action.title, "<script>literal</script>") == 0);
            UmiStatus edit_status = strcmp(mode, "disabled") == 0 ? UMI_STATUS_PERMISSION_DENIED
                                    : (strcmp(mode, "command") == 0 || strcmp(mode, "edit-command") == 0 ||
                                       strcmp(mode, "deferred") == 0 || strcmp(mode, "resource") == 0)
                                        ? UMI_STATUS_NOT_IMPLEMENTED
                                        : UMI_STATUS_OK;
            UmiLanguageWorkspaceEditCatalogue *edits = NULL;
            CHECK(UmiLanguageCodeActionCatalogueReadEdits(catalogue, 0U, NULL, &edits) == edit_status);
            if (edit_status == UMI_STATUS_OK)
            {
                CHECK(UmiLanguageWorkspaceEditCatalogueCount(edits) == 1U);
                UmiLanguageWorkspaceDocumentChange document;
                CHECK(UmiLanguageWorkspaceEditCatalogueDocument(edits, 0U, &document) == UMI_STATUS_OK &&
                      document.has_version && document.version == 1);
                CHECK(strcmp(document.uri, strcmp(mode, "external") == 0 ? "file:///workspace/other.c"
                                                                         : request.document_uri) == 0);
                if (strcmp(mode, "annotation") == 0)
                {
                    UmiLanguageWorkspaceChangeAnnotation annotation;
                    CHECK(UmiLanguageWorkspaceEditCatalogueAnnotation(edits, 0U, &annotation) ==
                              UMI_STATUS_OK &&
                          annotation.needs_confirmation);
                }
            }
            else
                CHECK(edits == NULL);
            UmiLanguageWorkspaceEditCatalogueDestroy(edits);
        }
    }
    else
        CHECK(catalogue == NULL);
    if (strcmp(mode, "cancel") == 0)
        CHECK(!report.started);
    UmiLanguageCodeActionCatalogueDestroy(catalogue);
    umi_cancellation_token_destroy(cancel);
    return 0;
}
#include "../native_process/utf8_entry.inc"
