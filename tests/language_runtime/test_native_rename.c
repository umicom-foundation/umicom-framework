/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_native_rename.c
 * PURPOSE: Exercise rename proposals and preparation through a native child process.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#include "umicom/language_runtime/rename_query.h"
static int ProgramMain(int argc, char **argv)
{
    CHECK(argc == 3);
    const char *mode = argv[1];
    const char *known[] = {"valid",
                           "unicode",
                           "empty",
                           "error",
                           "invalid",
                           "timeout",
                           "encoding",
                           "shutdown",
                           "cancel",
                           "stale",
                           "null-version",
                           "annotation",
                           "outside",
                           "resource",
                           "external",
                           "prepare",
                           "prepare-placeholder",
                           "prepare-null",
                           "prepare-default",
                           "prepare-invalid",
                           "prepare-timeout"};

    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    UmiLanguageServerProfile profile = {0};
    strcpy(profile.id, "native-rename-peer");
    CHECK(strlen(argv[2]) < sizeof(profile.executable));
    strcpy(profile.executable, argv[2]);
    profile.enabled = 1;
    (void)snprintf(profile.arguments, sizeof(profile.arguments), "rename-%s", mode);
    UmiLanguageRenameRequest request = {
        "file:///workspace", "file:///workspace/main.c", "c", "abc", 3U, 1U, 3000U, "total"};
    if (strcmp(mode, "unicode") == 0)
    {
        request.source = "a\xf0\x9f\x8c\x8d"
                         "c";
        request.source_bytes = 6U;
        request.caret = 5U;
    }
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(mode, "error") == 0 || strcmp(mode, "shutdown") == 0)
        expected = UMI_STATUS_UNAVAILABLE;
    if (strcmp(mode, "invalid") == 0 || strcmp(mode, "prepare-invalid") == 0)
        expected = UMI_STATUS_PARSE_ERROR;
    if (strcmp(mode, "encoding") == 0 || strcmp(mode, "resource") == 0 ||
        strcmp(mode, "prepare-default") == 0)
        expected = UMI_STATUS_NOT_IMPLEMENTED;
    if (strcmp(mode, "stale") == 0)
        expected = UMI_STATUS_INVALID_STATE;
    if (strcmp(mode, "outside") == 0)
        expected = UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(mode, "prepare-null") == 0)
        expected = UMI_STATUS_NOT_FOUND;
    if (strcmp(mode, "timeout") == 0 || strcmp(mode, "prepare-timeout") == 0)
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
    UmiLanguageRenameReport report;
    UmiLanguageWorkspaceEditCatalogue *catalogue = NULL;
    UmiStatus status = UmiLanguageRenameQueryNative(&profile, NULL, &request, cancel, &report, &catalogue);
    if (status != expected)
        fprintf(stderr, "mode %s: status %d expected %d\n", mode, (int)status, (int)expected);
    CHECK(status == expected);
    if (expected == UMI_STATUS_OK)
    {
        size_t count = strcmp(mode, "empty") == 0 ? 0U : 1U;
        CHECK(report.initialized && report.document_opened && report.documents == count &&
              report.shutdown_status == UMI_STATUS_OK);
        CHECK(UmiLanguageWorkspaceEditCatalogueCount(catalogue) == count);
        if (count != 0U)
        {
            UmiLanguageWorkspaceDocumentChange document;
            UmiLanguageWorkspaceTextChange edit;
            CHECK(UmiLanguageWorkspaceEditCatalogueDocument(catalogue, 0U, &document) == UMI_STATUS_OK);
            CHECK(strcmp(document.uri, strcmp(mode, "external") == 0 ? "file:///workspace/other.c"
                                                                     : request.document_uri) == 0);
            CHECK(document.has_version == (strcmp(mode, "null-version") != 0));
            CHECK(UmiLanguageWorkspaceEditCatalogueEdit(catalogue, 0U, 0U, &edit) == UMI_STATUS_OK &&
                  strcmp(edit.text, "total") == 0);
            if (strcmp(mode, "annotation") == 0)
            {
                UmiLanguageWorkspaceChangeAnnotation annotation;
                CHECK(edit.annotation == 0U);
                CHECK(UmiLanguageWorkspaceEditCatalogueAnnotation(catalogue, 0U, &annotation) ==
                          UMI_STATUS_OK &&
                      annotation.needs_confirmation);
            }
        }
    }
    else
        CHECK(catalogue == NULL);
    if (strcmp(mode, "cancel") == 0)
        CHECK(!report.started);
    UmiLanguageWorkspaceEditCatalogueDestroy(catalogue);
    umi_cancellation_token_destroy(cancel);
    return 0;
}
#include "../native_process/utf8_entry.inc"
