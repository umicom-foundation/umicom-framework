/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_recovery_coordinator.c
 * PURPOSE: Verify that recovery copies visible text and restores independently of original source identities.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"
#include "umicom/document/recovery_coordinator.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1], *cases[] = {"capture",
                                            "pending-typing",
                                            "capture-read-only",
                                            "selection",
                                            "unicode",
                                            "crlf",
                                            "large",
                                            "empty",
                                            "ownership",
                                            "other-active",
                                            "invalid-selection",
                                            "invalid-identity",
                                            "missing",
                                            "closed",
                                            "cancelled",
                                            "invalid-key",
                                            "null-capture",
                                            "restore",
                                            "restore-position",
                                            "restore-twice",
                                            "restore-existing",
                                            "restore-no-history",
                                            "null-restore"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(mode, cases[i]) == 0)
            known = 1;
    CHECK(known);
    const char *key = "0123456789abcdef0123456789abcdef", *source = "one\ntwo";
    char *large = NULL;
    size_t cursor = 0U, selected = 0U;
    if (strcmp(mode, "selection") == 0 || strcmp(mode, "restore-position") == 0)
    {
        cursor = 4U;
        selected = 3U;
    }
    if (strcmp(mode, "unicode") == 0)
    {
        source = "a\xe9\x9b\xaa";
        cursor = 1U;
        selected = 3U;
    }
    if (strcmp(mode, "crlf") == 0)
    {
        source = "a\r\nb";
        cursor = 3U;
        selected = 1U;
    }
    if (strcmp(mode, "large") == 0)
    {
        large = malloc(32769U);
        CHECK(large);
        memset(large, 'x', 32768U);
        large[32768] = '\0';
        source = large;
        cursor = 32768U;
    }
    if (strcmp(mode, "empty") == 0)
        source = "";
    ReplacementFixture f = {0};
    CHECK(Start(&f, source) == 0);
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f.workbench);
    UmiUiDocumentViewSnapshot view;
    CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK);
    view.cursor_offset = cursor;
    view.selection_length = selected;
    view.read_only = strcmp(mode, "capture-read-only") == 0;
    if (strcmp(mode, "invalid-selection") == 0)
        view.cursor_offset = strlen(source) + 1U;
    if (strcmp(mode, "invalid-identity") == 0)
        strcpy(view.document_id, "different");
    CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
    if (strcmp(mode, "pending-typing") == 0)
    {
        source = "pending unsaved typing";
        CHECK(Draft(&f, source) == UMI_STATUS_OK);
    }
    if (strcmp(mode, "other-active") == 0)
        CHECK(umi_document_coordinator_new(f.documents, "other.c", NULL, 0U) == UMI_STATUS_OK);
    UmiDocumentSnapshot before;
    CHECK(umi_document_store_snapshot(f.store, f.id, &before) == UMI_STATUS_OK);
    UmiDocumentWorkingCopySnapshot active_before;
    CHECK(umi_document_coordinator_active_snapshot(f.documents, &active_before) == UMI_STATUS_OK);
    if (strcmp(mode, "closed") == 0)
        CHECK(umi_document_coordinator_close_active(f.documents, 1) == UMI_STATUS_OK);
    UmiCancellationToken *cancel = NULL;
    if (strcmp(mode, "cancelled") == 0)
    {
        CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
        umi_cancellation_token_request(cancel);
    }
    UmiDocumentRecoveryDraft *draft = NULL;
    UmiStatus status = UmiDocumentCoordinatorCaptureRecovery(
        strcmp(mode, "null-capture") == 0 ? NULL : f.documents,
        strcmp(mode, "missing") == 0 ? UINT64_MAX : f.id, strcmp(mode, "invalid-key") == 0 ? "invalid" : key,
        cancel, &draft);
    int rejection = strcmp(mode, "invalid-selection") == 0 || strcmp(mode, "invalid-identity") == 0 ||
                    strcmp(mode, "closed") == 0 || strcmp(mode, "cancelled") == 0 ||
                    strcmp(mode, "invalid-key") == 0 || strcmp(mode, "missing") == 0 ||
                    strcmp(mode, "null-capture") == 0;
    if (rejection)
    {
        CHECK(status != UMI_STATUS_OK && draft == NULL);
        goto done;
    }
    CHECK(status == UMI_STATUS_OK && draft != NULL);
    UmiDocumentSnapshot after;
    CHECK(umi_document_store_snapshot(f.store, f.id, &after) == UMI_STATUS_OK &&
          before.revision == after.revision && before.saved_revision == after.saved_revision &&
          before.dirty == after.dirty);
    UmiDocumentWorkingCopySnapshot active_after;
    CHECK(umi_document_coordinator_active_snapshot(f.documents, &active_after) == UMI_STATUS_OK &&
          active_after.document_id == active_before.document_id &&
          active_after.undo_count == active_before.undo_count);
    UmiDocumentRecoveryInfo info;
    const char *text = NULL;
    size_t bytes = 0U;
    CHECK(UmiDocumentRecoveryDraftInspect(draft, &info) == UMI_STATUS_OK && info.cursor_offset == cursor &&
          info.selection_bytes == selected);
    CHECK(UmiDocumentRecoveryDraftRead(draft, &text, &bytes) == UMI_STATUS_OK && bytes == strlen(source) &&
          strcmp(text, source) == 0);
    if (strcmp(mode, "ownership") == 0)
    {
        CHECK(Draft(&f, "changed later") == UMI_STATUS_OK);
        CHECK(umi_document_coordinator_close_active(f.documents, 1) == UMI_STATUS_OK);
        CHECK(strcmp(text, source) == 0);
        goto done;
    }
    if (strcmp(mode, "null-restore") == 0)
    {
        UmiDocumentId id = 99U;
        CHECK(UmiDocumentCoordinatorRestoreRecovery(NULL, draft, &id) == UMI_STATUS_INVALID_ARGUMENT &&
              id == 0U);
        CHECK(UmiDocumentCoordinatorRestoreRecovery(f.documents, NULL, &id) == UMI_STATUS_INVALID_ARGUMENT &&
              id == 0U);
        goto done;
    }
    if (strncmp(mode, "restore", 7U) == 0 || strcmp(mode, "unicode") == 0 || strcmp(mode, "crlf") == 0 ||
        strcmp(mode, "large") == 0 || strcmp(mode, "empty") == 0)
    {
        if (strcmp(mode, "restore-existing") == 0)
        {
            /* A descriptive path is not an authority to bind or overwrite the
             * original working copy. The fixture never creates that path. */
            UmiDocumentRecoveryDraft *with_path = NULL;
            strcpy(info.source_path, "/original/file.c");
            CHECK(UmiDocumentRecoveryDraftCreate(&info, text, bytes, NULL, &with_path) == UMI_STATUS_OK);
            UmiDocumentRecoveryDraftDestroy(draft);
            draft = with_path;
        }
        size_t count = umi_document_coordinator_count(f.documents);
        UmiDocumentId restored = 0U;
        CHECK(UmiDocumentCoordinatorRestoreRecovery(f.documents, draft, &restored) == UMI_STATUS_OK &&
              restored != f.id && restored != 0U &&
              umi_document_coordinator_count(f.documents) == count + 1U);
        UmiDocumentWorkingCopySnapshot copy;
        CHECK(umi_document_coordinator_active_snapshot(f.documents, &copy) == UMI_STATUS_OK &&
              copy.document_id == restored && !copy.has_path && copy.dirty && copy.undo_count == 0U &&
              copy.redo_count == 0U);
        CHECK(umi_ui_document_view_model_find(views, copy.view_id, &view) == UMI_STATUS_OK &&
              view.cursor_offset == cursor && view.selection_length == selected && view.dirty &&
              !view.read_only);
        char *restored_text = NULL;
        size_t restored_bytes = 0U;
        CHECK(UmiUiDocumentViewModelCopyText(views, copy.view_id, &restored_text, &restored_bytes) ==
                  UMI_STATUS_OK &&
              restored_bytes == strlen(source) && strcmp(restored_text, source) == 0);
        UmiUiDocumentViewModelFreeText(restored_text);
        CHECK(ExpectText(&f, source) == 0);
        if (strcmp(mode, "restore-twice") == 0)
        {
            UmiDocumentId second = 0U;
            CHECK(UmiDocumentCoordinatorRestoreRecovery(f.documents, draft, &second) == UMI_STATUS_OK &&
                  second != restored && second != f.id &&
                  umi_document_coordinator_count(f.documents) == count + 2U);
        }
        if (strcmp(mode, "restore-no-history") == 0)
        {
            CHECK(UmiDocumentCoordinatorUndo(f.documents, restored) != UMI_STATUS_OK);
            CHECK(umi_document_coordinator_active_snapshot(f.documents, &copy) == UMI_STATUS_OK &&
                  copy.document_id == restored);
        }
    }
done:
    UmiDocumentRecoveryDraftDestroy(draft);
    umi_cancellation_token_destroy(cancel);
    Stop(&f);
    free(large);
    return 0;
}
