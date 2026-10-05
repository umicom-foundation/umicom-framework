/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_native_symbols.c
 * PURPOSE: Check document-symbol outlines through a native child and an independent protocol peer.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#include "umicom/language_runtime/symbol_query.h"
static int ProgramMain(int argc, char **argv)
{
    CHECK(argc == 3);
    const char *mode = argv[1];
    const char *known[] = {"valid",  "unicode", "empty",     "flat",        "hierarchy",
                           "error",  "invalid", "timeout",   "encoding",    "shutdown",
                           "cancel", "range",   "surrogate", "unknown-kind"};
    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    UmiLanguageServerProfile profile = {0};
    strcpy(profile.id, "native-symbols-peer");
    CHECK(strlen(argv[2]) < sizeof(profile.executable));
    strcpy(profile.executable, argv[2]);
    profile.enabled = 1;
    (void)snprintf(profile.arguments, sizeof(profile.arguments), "symbols-%s", mode);
    UmiLanguageSymbolRequest request = {
        "file:///workspace", "file:///workspace/main.c", "c", "pu", 2U, 3000U};
    if ((strcmp(mode, "unicode") == 0 || strcmp(mode, "surrogate") == 0))
    {
        request.source = "\xf0\x9f\x98\x80";
        request.source_bytes = 4U;
    }
    if (strcmp(mode, "empty") == 0)
    {
        request.source = "";
        request.source_bytes = 0U;
    }
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(mode, "error") == 0 || strcmp(mode, "shutdown") == 0)
        expected = UMI_STATUS_UNAVAILABLE;
    if ((strcmp(mode, "range") == 0 || strcmp(mode, "surrogate") == 0))
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
    UmiLanguageSymbolReport report;
    UmiLanguageSymbolCatalogue *preview = NULL;
    CHECK(UmiLanguageSymbolQueryNative(&profile, NULL, &request, cancel, &report, &preview) == expected);
    if (expected == UMI_STATUS_OK)
    {
        size_t count = strcmp(mode, "empty") == 0 ? 0U : strcmp(mode, "hierarchy") == 0 ? 2U : 1U;
        CHECK(report.initialized && report.document_opened && report.symbols == count &&
              report.shutdown_status == UMI_STATUS_OK);
        CHECK(UmiLanguageSymbolCatalogueCount(preview) == count);
        if (count != 0U)
        {
            UmiLanguageSymbol symbol;
            CHECK(UmiLanguageSymbolCatalogueAt(preview, count - 1U, &symbol) == UMI_STATUS_OK);
            CHECK(strcmp(symbol.name, "pu") == 0 && strcmp(symbol.location.uri, request.document_uri) == 0);
            CHECK(symbol.depth == (strcmp(mode, "hierarchy") == 0 ? 1U : 0U));
            CHECK(symbol.hierarchical == (strcmp(mode, "flat") != 0));
            CHECK(symbol.kind == (strcmp(mode, "unknown-kind") == 0 ? 500 : 12));
        }
    }
    else
        CHECK(preview == NULL);
    if (strcmp(mode, "cancel") == 0)
        CHECK(!report.started);
    UmiLanguageSymbolCatalogueDestroy(preview);
    umi_cancellation_token_destroy(cancel);
    return 0;
}
#include "../native_process/utf8_entry.inc"
