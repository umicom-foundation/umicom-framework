/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_native_rename_sources.c
 * PURPOSE: Exercise additional unsaved source synchronization through an independent native peer.
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
    const char *known[] = {"valid", "all",     "prepare",     "unicode",       "outside",
                           "stale", "unknown", "query-error", "shutdown-error"};
    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    UmiLanguageServerProfile profile = {0};
    strcpy(profile.id, "native-rename-sources-peer");
    CHECK(strlen(argv[2]) < sizeof(profile.executable));
    strcpy(profile.executable, argv[2]);
    profile.enabled = 1;
    (void)snprintf(profile.arguments, sizeof(profile.arguments), "rename-sources-%s", mode);
    UmiLanguageRenameRequest request = {
        "file:///workspace", "file:///workspace/main.c", "c", "source main", 11U, 2U, 3000U, "renamed"};
    UmiLanguageQuerySource sources[2] = {{"file:///workspace/second.c", "c", "source second", 13U},
                                         {"file:///workspace/third.c", "c", "source third", 12U}};
    if (strcmp(mode, "unicode") == 0)
    {
        sources[0].source = "source second caf\xc3\xa9";
        sources[0].source_bytes = strlen(sources[0].source);
    }
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(mode, "outside") == 0)
        expected = UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(mode, "stale") == 0)
        expected = UMI_STATUS_INVALID_STATE;
    if (strcmp(mode, "query-error") == 0 || strcmp(mode, "shutdown-error") == 0)
        expected = UMI_STATUS_UNAVAILABLE;
    UmiLanguageRenameReport report;
    UmiLanguageWorkspaceEditCatalogue *catalogue = NULL;
    UmiStatus status = UmiLanguageRenameQueryNativeWithSources(&profile, NULL, &request, sources, 2U, NULL,
                                                               &report, &catalogue);
    if (status != expected)
        fprintf(stderr, "%s: %d expected %d\n", mode, (int)status, (int)expected);
    CHECK(status == expected);
    CHECK(report.started && report.initialized && report.document_opened &&
          report.close_status == UMI_STATUS_OK);
    if (expected == UMI_STATUS_OK)
    {
        CHECK(report.shutdown_status == UMI_STATUS_OK && catalogue != NULL);
        CHECK(UmiLanguageWorkspaceEditCatalogueCount(catalogue) == (strcmp(mode, "all") == 0 ? 3U : 1U));
        if (strcmp(mode, "all") != 0)
        {
            UmiLanguageWorkspaceDocumentChange document;
            CHECK(UmiLanguageWorkspaceEditCatalogueDocument(catalogue, 0U, &document) == UMI_STATUS_OK);
            CHECK(strcmp(document.uri, strcmp(mode, "unknown") == 0 ? "file:///workspace/unknown.c"
                                                                    : "file:///workspace/second.c") == 0);
        }
    }
    else
        CHECK(catalogue == NULL);
    UmiLanguageWorkspaceEditCatalogueDestroy(catalogue);
    return 0;
}
#include "../native_process/utf8_entry.inc"
