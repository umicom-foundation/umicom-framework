/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_native_signature.c
 * PURPOSE: Check parameter help through a native child and an independent protocol peer.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#include "umicom/language_runtime/signature_query.h"
static int ProgramMain(int argc, char **argv)
{
    CHECK(argc == 3);
    const char *mode = argv[1];
    const char *known[] = {"valid",   "unicode",  "empty",    "markup", "error",     "invalid",
                           "timeout", "encoding", "shutdown", "cancel", "surrogate", "overloads"};
    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    UmiLanguageServerProfile profile = {0};
    strcpy(profile.id, "native-signature-peer");
    CHECK(strlen(argv[2]) < sizeof(profile.executable));
    strcpy(profile.executable, argv[2]);
    profile.enabled = 1;
    (void)snprintf(profile.arguments, sizeof(profile.arguments), "signature-%s", mode);
    UmiLanguageSignatureRequest request = {
        "file:///workspace", "file:///workspace/main.c", "c", "pu", 2U, 2U, 3000U};
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
    if (strcmp(mode, "surrogate") == 0)
        expected = UMI_STATUS_PARSE_ERROR;
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
    UmiLanguageSignatureReport report;
    UmiLanguageSignatureCatalogue *preview = NULL;
    CHECK(UmiLanguageSignatureQueryNative(&profile, NULL, &request, cancel, &report, &preview) == expected);
    if (expected == UMI_STATUS_OK)
    {
        size_t count = strcmp(mode, "empty") == 0 ? 0U : strcmp(mode, "overloads") == 0 ? 2U : 1U;
        CHECK(report.initialized && report.document_opened && report.signatures == count &&
              report.shutdown_status == UMI_STATUS_OK);
        CHECK(UmiLanguageSignatureCatalogueCount(preview) == count);
        if (count != 0U)
        {
            UmiLanguageSignature signature;
            size_t active = UmiLanguageSignatureCatalogueActive(preview);
            CHECK(active == (strcmp(mode, "overloads") == 0 ? 1U : 0U));
            CHECK(UmiLanguageSignatureCatalogueAt(preview, active, &signature) == UMI_STATUS_OK);
            CHECK(signature.active_parameter ==
                  (strcmp(mode, "markup") == 0 || strcmp(mode, "overloads") == 0 ? 0U : 1U));
            if (strcmp(mode, "markup") == 0)
                CHECK(strcmp(signature.documentation.text, "<script>literal</script>") == 0);
        }
    }
    else
        CHECK(preview == NULL);
    if (strcmp(mode, "cancel") == 0)
        CHECK(!report.started);
    UmiLanguageSignatureCatalogueDestroy(preview);
    umi_cancellation_token_destroy(cancel);
    return 0;
}
#include "../native_process/utf8_entry.inc"
