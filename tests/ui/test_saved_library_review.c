/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui/test_saved_library_review.c
 * PURPOSE: Exercise exact saved-layout evidence, scope, recovery and stale refusal.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workspace_library_exchange.h"
#include "umicom/workbench_layout_data/key_codec.h"
#include "umicom/test_runtime/check.h"
#include <stdlib.h>
#include <string.h>
#define CHECK(x) UMI_TEST_REQUIRE(x)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
static const UmiUiWorkspaceCheckpointScope scope = {"test.saved", "desktop", "test.saved."};
static int Rename(UmiUiWorkspaceCustomisation *model, const char *title)
{
    const UmiUiWorkspaceLibraryPolicy policy = {"test.saved."};
    const UmiUiWorkspaceLibraryRequest request = {UMI_UI_WORKSPACE_LIBRARY_RENAME,
        "test.saved.main", NULL, title, model->revision, false};
    OK(umi_ui_workspace_library_apply(model, &policy, &request, NULL)); return 0;
}
int main(int argc, char **argv)
{
    const char *cases[] = {"evidence", "frozen", "stale", "foreign", "recovery", "missing", "transaction", "invalid"};
    if (argc != 2) return 2;
    int known = 0; for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i) if (!strcmp(argv[1], cases[i])) known = 1;
    if (!known) return 2;
    UmiDataServer *server = NULL;
    UmiUiWorkspaceCustomisation *model = calloc(1U, sizeof(*model));
    UmiUiWorkspaceCustomisation *candidate = malloc(sizeof(*candidate)), *before = malloc(sizeof(*before));
    UmiUiWorkspaceCustomisation *writer = malloc(sizeof(*writer));
    UmiUiWorkspaceLibraryImport *review = NULL;
    CHECK(model && candidate && before && writer); OK(umi_data_server_create_memory(&server));
    umi_ui_workspace_customisation_init(model);
    OK(umi_ui_workspace_customisation_create_blank_layout(model, "test.saved.main", "Saved café"));
    if (!strcmp(argv[1], "missing")) {
        CHECK(umi_ui_workspace_library_checkpoint_review(server, &scope, model, &review) == UMI_STATUS_NOT_FOUND);
        CHECK(review == NULL);
    } else {
        OK(umi_ui_workspace_library_checkpoint_save(server, &scope, model, 42U, 0U, NULL));
        *writer = *model;
        CHECK(Rename(model, "Current workspace") == 0); *before = *model;
        if (!strcmp(argv[1], "recovery")) {
            CHECK(Rename(writer, "Newer stored workspace") == 0);
            OK(umi_ui_workspace_library_checkpoint_save(server, &scope, writer, 99U, 1U, NULL));
            char key[UMI_WORKBENCH_LAYOUT_DATA_KEY_CAPACITY];
            OK(umi_workbench_layout_data_key_build(UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_MANIFEST,
                "test.saved@desktop@library-primary", NULL, 0U, 0U, key, sizeof key));
            OK(umi_data_server_set(server, key, "damaged manifest"));
        }
        OK(umi_ui_workspace_library_checkpoint_review(server, &scope, model, &review));
        CHECK(memcmp(model, before, sizeof(*model)) == 0);
        const UmiUiWorkspaceLibraryPreview *summary = umi_ui_workspace_library_import_summary(review);
        CHECK(summary && summary->report.checkpoint.saved_at_ns == 42U && !summary->report.checkpoint.durable);
        CHECK(summary->report.checkpoint.recovered_last_good == (!strcmp(argv[1], "recovery")));
        CHECK(!strcmp(umi_ui_workspace_library_import_layout(review, false, 0U)->name, "Current workspace"));
        CHECK(!strcmp(umi_ui_workspace_library_import_layout(review, true, 0U)->name, "Saved café"));
        *candidate = *model;
        if (!strcmp(argv[1], "frozen")) {
            CHECK(Rename(writer, "Later saved content") == 0);
            OK(umi_ui_workspace_library_checkpoint_save(server, &scope, writer, 100U, 1U, NULL));
            /* Destroying storage cannot invalidate owned geometry or cause a reread. */
            umi_data_server_destroy(server); server = NULL;
            OK(umi_ui_workspace_library_import_candidate(review, &scope, model, candidate));
            CHECK(!strcmp(candidate->layouts[0].name, "Saved café"));
        } else if (!strcmp(argv[1], "foreign")) {
            *writer = *model;
            CHECK(umi_ui_workspace_library_import_candidate(review, &scope, writer, candidate) == UMI_STATUS_PERMISSION_DENIED);
            CHECK(memcmp(candidate, before, sizeof(*candidate)) == 0);
        } else if (!strcmp(argv[1], "stale")) {
            CHECK(Rename(model, "Changed after review") == 0);
            CHECK(umi_ui_workspace_library_import_candidate(review, &scope, model, candidate) == UMI_STATUS_INVALID_STATE);
            CHECK(memcmp(candidate, before, sizeof(*candidate)) == 0);
        } else if (!strcmp(argv[1], "transaction")) {
            UmiUiWorkspaceLibraryImport *previous = review;
            OK(umi_data_server_begin(server));
            CHECK(umi_ui_workspace_library_checkpoint_review(server, &scope, model, &review) == UMI_STATUS_BUSY);
            CHECK(review == previous && umi_data_server_in_transaction(server)); OK(umi_data_server_rollback(server));
        } else if (!strcmp(argv[1], "invalid")) {
            UmiUiWorkspaceLibraryImport *previous = review;
            CHECK(umi_ui_workspace_library_checkpoint_review(NULL, &scope, model, &review) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(umi_ui_workspace_library_checkpoint_review(server, NULL, model, &review) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(umi_ui_workspace_library_checkpoint_review(server, &scope, model, NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(review == previous);
        } else {
            OK(umi_ui_workspace_library_import_candidate(review, &scope, model, candidate));
            CHECK(!strcmp(candidate->layouts[0].name, "Saved café"));
            CHECK(candidate->revision == model->revision + 1U);
        }
    }
    umi_ui_workspace_library_import_destroy(review); umi_data_server_destroy(server);
    free(writer); free(before); free(candidate); free(model); return 0;
}
