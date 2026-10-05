/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_native_action_context.c
 * PURPOSE: Check same-session diagnostic-backed actions through a native child and an independent protocol peer.
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
    CHECK(strcmp(mode, "valid") == 0 || strcmp(mode, "empty") == 0 || strcmp(mode, "missing") == 0 ||
          strcmp(mode, "invalid") == 0);
    UmiLanguageServerProfile profile = {0};
    strcpy(profile.id, "action-context-peer");
    CHECK(strlen(argv[2]) < sizeof(profile.executable));
    strcpy(profile.executable, argv[2]);
    profile.enabled = 1;
    (void)snprintf(profile.arguments, sizeof(profile.arguments), "actions-context-%s", mode);
    UmiLanguageCodeActionRequest request = {
        "file:///workspace", "file:///workspace/main.c", "c", "abc", 3U, 0U, 3U, 3000U};
    UmiStatus expected = strcmp(mode, "missing") == 0   ? UMI_STATUS_TIMEOUT
                         : strcmp(mode, "invalid") == 0 ? UMI_STATUS_PARSE_ERROR
                                                        : UMI_STATUS_OK;
    if (strcmp(mode, "missing") == 0)
        request.timeout_ms = 100U;
    UmiLanguageCodeActionReport report;
    UmiLanguageCodeActionCatalogue *catalogue = NULL;
    CHECK(UmiLanguageCodeActionQueryNativeWithDiagnostics(&profile, NULL, &request, NULL, &report,
                                                          &catalogue) == expected);
    if (expected == UMI_STATUS_OK)
        CHECK(report.initialized && report.document_opened && report.actions == 1U &&
              UmiLanguageCodeActionCatalogueCount(catalogue) == 1U && report.close_status == UMI_STATUS_OK &&
              report.shutdown_status == UMI_STATUS_OK);
    else
        CHECK(catalogue == NULL);
    UmiLanguageCodeActionCatalogueDestroy(catalogue);
    return 0;
}
#include "../native_process/utf8_entry.inc"
