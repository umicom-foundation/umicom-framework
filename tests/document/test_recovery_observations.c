/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_recovery_observations.c
 * PURPOSE: Observe source identities without saving drafts, changing Undo or treating caret motion as new source.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"
#include "umicom/document/recovery_schedule.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1],
               *cases[] = {"pending",  "synchronized", "caret-only", "text-change", "clean",      "read-only",
                           "capacity", "identity",     "closed",     "empty",       "null-output"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(mode, cases[i]) == 0)
            known = 1;
    CHECK(known);
    ReplacementFixture f = {0};
    CHECK(Start(&f, "source") == 0);
    UmiDocumentRecoveryObservation before[2], after[2];
    size_t count = 0U;
    CHECK(UmiDocumentCoordinatorObserveRecovery(f.documents, before, 2U, &count) == UMI_STATUS_OK &&
          count == 1U && before[0].dirty);
    UmiDocumentWorkingCopySnapshot old;
    CHECK(umi_document_coordinator_active_snapshot(f.documents, &old) == UMI_STATUS_OK);
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f.workbench);
    UmiUiDocumentViewSnapshot view;
    CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK);
    if (strcmp(mode, "pending") == 0 || strcmp(mode, "text-change") == 0)
        CHECK(Draft(&f, "new typing") == UMI_STATUS_OK);
    if (strcmp(mode, "synchronized") == 0)
    {
        CHECK(Draft(&f, "new typing") == UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorSyncDocument(f.documents, f.id) == UMI_STATUS_OK);
    }
    if (strcmp(mode, "caret-only") == 0)
    {
        view.cursor_offset = 1U;
        view.selection_length = 2U;
        CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
    }
    if (strcmp(mode, "read-only") == 0)
    {
        view.read_only = 1;
        CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
    }
    if (strcmp(mode, "clean") == 0)
    {
#ifdef _WIN32
        const char *path = "C:/recovery-fixture/source.c";
#else
        const char *path = "/recovery-fixture/source.c";
#endif
        CHECK(umi_document_store_mark_saved_as(f.store, f.id, path) == UMI_STATUS_OK);
        view.dirty = 0;
        CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
    }
    if (strcmp(mode, "identity") == 0)
    {
        strcpy(view.document_id, "different");
        CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
    }
    if (strcmp(mode, "closed") == 0 || strcmp(mode, "empty") == 0)
        CHECK(umi_document_coordinator_close_active(f.documents, 1) == UMI_STATUS_OK);
    memset(after, 0x5a, sizeof(after));
    UmiDocumentRecoveryObservation unchanged[2];
    memcpy(unchanged, after, sizeof(after));
    count = 99U;
    UmiStatus status =
        UmiDocumentCoordinatorObserveRecovery(f.documents, strcmp(mode, "null-output") == 0 ? NULL : after,
                                              strcmp(mode, "capacity") == 0 ? 0U : 2U, &count);
    if (strcmp(mode, "identity") == 0 || strcmp(mode, "capacity") == 0 || strcmp(mode, "null-output") == 0)
    {
        CHECK(status != UMI_STATUS_OK && count == 0U && memcmp(after, unchanged, sizeof(after)) == 0);
        goto done;
    }
    CHECK(status == UMI_STATUS_OK);
    if (strcmp(mode, "closed") == 0 || strcmp(mode, "empty") == 0)
    {
        CHECK(count == 0U);
        goto done;
    }
    CHECK(count == 1U && after[0].document_id == f.id);
    if (strcmp(mode, "clean") == 0)
        CHECK(!after[0].dirty);
    else
        CHECK(after[0].dirty);
    if (strcmp(mode, "text-change") == 0 || strcmp(mode, "pending") == 0)
        CHECK(after[0].text_revision != before[0].text_revision &&
              after[0].store_revision == before[0].store_revision);
    if (strcmp(mode, "synchronized") == 0)
        CHECK(after[0].text_revision != before[0].text_revision &&
              after[0].store_revision != before[0].store_revision);
    if (strcmp(mode, "caret-only") == 0)
        CHECK(after[0].text_revision == before[0].text_revision &&
              after[0].store_revision == before[0].store_revision);
    UmiDocumentWorkingCopySnapshot current;
    CHECK(umi_document_coordinator_active_snapshot(f.documents, &current) == UMI_STATUS_OK);
    if (strcmp(mode, "synchronized") != 0)
        CHECK(current.undo_count == old.undo_count);
done:
    Stop(&f);
    return 0;
}
