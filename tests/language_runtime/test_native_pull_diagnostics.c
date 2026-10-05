/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_native_pull_diagnostics.c
 * PURPOSE: Check explicit diagnostic requests through a native child and an independent protocol peer.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#include "umicom/language_runtime/diagnostic_query.h"

static int ProgramMain(int argc, char **argv)
{
    CHECK(argc == 3);
    const char *mode = argv[1];
    const char *known[] = {"valid",    "unicode",   "empty",          "literal",     "invalid",    "timeout",
                           "encoding", "shutdown",  "cancel",         "surrogate",   "outside",    "related",
                           "choices",  "unchanged", "related-report", "unsupported", "query-error"};
    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    UmiLanguageServerProfile profile = {0};
    strcpy(profile.id, "native-diagnostic-peer");
    CHECK(strlen(argv[2]) < sizeof(profile.executable));
    strcpy(profile.executable, argv[2]);
    profile.enabled = 1;
    (void)snprintf(profile.arguments, sizeof(profile.arguments), "pull-diagnostics-%s", mode);
    UmiLanguageDiagnosticRequest request = {
        "file:///workspace", "file:///workspace/main.c", "c", "pu", 2U, 3000U};
    if (strcmp(mode, "unicode") == 0)
    {
        request.source = "\xf0\x9f\x98\x80";
        request.source_bytes = 4U;
    }
    if (strcmp(mode, "surrogate") == 0)
    {
        request.source = "ab\xf0\x9f\x98\x80";
        request.source_bytes = 6U;
    }
    if (strcmp(mode, "empty") == 0)
    {
        request.source = "";
        request.source_bytes = 0U;
    }
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(mode, "shutdown") == 0)
        expected = UMI_STATUS_UNAVAILABLE;
    if (strcmp(mode, "outside") == 0 || strcmp(mode, "surrogate") == 0)
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
    if (strcmp(mode, "unchanged") == 0)
        expected = UMI_STATUS_INVALID_STATE;
    if (strcmp(mode, "related-report") == 0 || strcmp(mode, "unsupported") == 0)
        expected = UMI_STATUS_NOT_IMPLEMENTED;
    if (strcmp(mode, "query-error") == 0)
        expected = UMI_STATUS_UNAVAILABLE;
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    if (strcmp(mode, "cancel") == 0)
    {
        umi_cancellation_token_request(cancel);
        expected = UMI_STATUS_CANCELLED;
    }
    UmiLanguageDiagnosticReport report;
    UmiLanguageDiagnosticCatalogue *catalogue = NULL;
    CHECK(UmiLanguageDiagnosticQueryPullNative(&profile, NULL, &request, cancel, &report, &catalogue) ==
          expected);
    if (expected == UMI_STATUS_OK)
    {
        size_t count = strcmp(mode, "empty") == 0 ? 0U : strcmp(mode, "choices") == 0 ? 2U : 1U;
        CHECK(report.initialized && report.document_opened && report.diagnostics == count &&
              report.shutdown_status == UMI_STATUS_OK);
        CHECK(UmiLanguageDiagnosticCatalogueCount(catalogue) == count);
        UmiLanguageDiagnosticPublication publication;
        CHECK(UmiLanguageDiagnosticCataloguePublication(catalogue, &publication) == UMI_STATUS_OK);
        CHECK(strcmp(publication.uri, request.document_uri) == 0 && !publication.has_version);
        const char *result_id = NULL;
        CHECK(UmiLanguageDiagnosticCataloguePullResultId(catalogue, &result_id) == UMI_STATUS_OK);
        CHECK(strcmp(result_id, "report-key") == 0);
        if (count != 0U)
        {
            UmiLanguageDiagnostic diagnostic;
            CHECK(UmiLanguageDiagnosticCatalogueAt(catalogue, 0U, &diagnostic) == UMI_STATUS_OK);
            if (strcmp(mode, "literal") == 0)
                CHECK(strcmp(diagnostic.message, "<script>literal</script>") == 0);
            if (strcmp(mode, "related") == 0)
                CHECK(diagnostic.related_count == 1U);
        }
    }
    else
        CHECK(catalogue == NULL);
    if (strcmp(mode, "cancel") == 0)
        CHECK(!report.started);
    UmiLanguageDiagnosticCatalogueDestroy(catalogue);
    umi_cancellation_token_destroy(cancel);
    return 0;
}
#include "../native_process/utf8_entry.inc"
