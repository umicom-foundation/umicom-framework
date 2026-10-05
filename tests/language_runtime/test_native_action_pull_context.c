/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_native_action_pull_context.c
 * PURPOSE: Exercise independent native multi-source action ordering, pull diagnostics and complete catalogues.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/code_action_query.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%d: %s\n", __LINE__, #x);                                                       \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
static int ProgramMain(int argc, char **argv)
{
    CHECK(argc == 3);
    const char *mode = argv[1];
    const char *cases[] = {"valid",     "all",           "unicode",       "outside",
                           "stale",     "unknown",       "query-error",   "shutdown-error",
                           "disabled",  "command",       "empty-context", "diagnostic-error",
                           "unchanged", "related-report"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(mode, cases[i]) == 0)
            known = 1;
    CHECK(known);
    UmiLanguageServerProfile profile = {0};
    strcpy(profile.id, "native-source-actions");
    profile.enabled = 1;
    CHECK(strlen(argv[2]) < sizeof(profile.executable));
    strcpy(profile.executable, argv[2]);
    int written = snprintf(profile.arguments, sizeof(profile.arguments), "actions-pull-%s", mode);
    CHECK(written > 0 && (size_t)written < sizeof(profile.arguments));
    UmiLanguageCodeActionRequest request = {
        "file:///workspace", "file:///workspace/main.c", "c", "source main", 11U, 0U, 6U, 3000U};
    const char *secondary = strcmp(mode, "unicode") == 0 ? "source second caf\xc3\xa9" : "source second";
    UmiLanguageQuerySource sources[] = {{"file:///workspace/second.c", "c", secondary, strlen(secondary)},
                                        {"file:///workspace/third.c", "c", "source third", 12U}};
    UmiLanguageCodeActionReport report;
    UmiLanguageCodeActionCatalogue *catalogue = NULL;
    UmiStatus expected = strcmp(mode, "query-error") == 0 || strcmp(mode, "shutdown-error") == 0
                             ? UMI_STATUS_UNAVAILABLE
                             : UMI_STATUS_OK;
    if (strcmp(mode, "diagnostic-error") == 0)
        expected = UMI_STATUS_UNAVAILABLE;
    if (strcmp(mode, "unchanged") == 0)
        expected = UMI_STATUS_INVALID_STATE;
    if (strcmp(mode, "related-report") == 0)
        expected = UMI_STATUS_NOT_IMPLEMENTED;
    CHECK(UmiLanguageCodeActionQueryNativeWithContext(&profile, NULL, &request, sources, 2U,
                                                      UMI_LANGUAGE_ACTION_DIAGNOSTICS_PULL, NULL, &report,
                                                      &catalogue) == expected);
    CHECK(report.started && report.initialized && report.document_opened &&
          report.close_status == UMI_STATUS_OK);
    if (expected == UMI_STATUS_OK)
    {
        CHECK(UmiLanguageCodeActionCatalogueCount(catalogue) == 1U);
        UmiLanguageWorkspaceEditCatalogue *edits = NULL;
        UmiStatus selected = strcmp(mode, "disabled") == 0  ? UMI_STATUS_PERMISSION_DENIED
                             : strcmp(mode, "command") == 0 ? UMI_STATUS_NOT_IMPLEMENTED
                                                            : UMI_STATUS_OK;
        CHECK(UmiLanguageCodeActionCatalogueReadEdits(catalogue, 0U, NULL, &edits) == selected);
        if (selected == UMI_STATUS_OK)
        {
            CHECK(UmiLanguageWorkspaceEditCatalogueCount(edits) == (strcmp(mode, "all") == 0 ? 3U : 1U));
            UmiLanguageWorkspaceDocumentChange change;
            CHECK(UmiLanguageWorkspaceEditCatalogueDocument(edits, 0U, &change) == UMI_STATUS_OK);
            if (strcmp(mode, "unknown") == 0)
                CHECK(strcmp(change.uri, "file:///workspace/unknown.c") == 0);
            else if (strcmp(mode, "outside") == 0 || strcmp(mode, "stale") == 0)
            {
                int32_t version = 1;
                UmiLanguageTextEditPreview *preview = NULL;
                UmiStatus invalid =
                    strcmp(mode, "stale") == 0 ? UMI_STATUS_INVALID_STATE : UMI_STATUS_INVALID_ARGUMENT;
                CHECK(UmiLanguageWorkspaceEditCataloguePreview(edits, 0U, secondary, strlen(secondary), 0U,
                                                               &version, NULL, &preview) == invalid &&
                      preview == NULL);
            }
        }
        else
            CHECK(edits == NULL);
        UmiLanguageWorkspaceEditCatalogueDestroy(edits);
    }
    else
        CHECK(catalogue == NULL);
    UmiLanguageCodeActionCatalogueDestroy(catalogue);
    return 0;
}
#include "../native_process/utf8_entry.inc"
