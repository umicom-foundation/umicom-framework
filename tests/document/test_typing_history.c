/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_typing_history.c
 * PURPOSE: Check captured native typing groups preserve positions and reject unrelated document state.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *name = argv[1], *cases[] = {"group",
                                            "selection",
                                            "unicode",
                                            "crlf",
                                            "empty",
                                            "unchanged",
                                            "pending-before-capture",
                                            "store-changed",
                                            "store-round-trip",
                                            "view-identity",
                                            "uri-changed",
                                            "read-only",
                                            "capture-read-only",
                                            "other-tab",
                                            "consumed",
                                            "closed",
                                            "null-owner",
                                            "null-capture",
                                            "other-owner",
                                            "invalid-selection"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(name, cases[i]) == 0)
            known = 1;
    CHECK(known);
    ReplacementFixture f = {0}, other = {0};
    const char *source = "a\nbb\nccc", *typed = "a\nXY\nccc";
    size_t cursor = 2U, selected = 0U, typed_cursor = 4U;
    UmiStatus wanted = UMI_STATUS_OK;
    if (strcmp(name, "selection") == 0)
        selected = 2U;
    if (strcmp(name, "unicode") == 0)
    {
        source = "a\n\xe9\x9b\xaa\nccc";
        selected = 3U;
    }
    if (strcmp(name, "crlf") == 0)
    {
        source = "a\r\nbb\r\nccc";
        typed = "a\r\nXY\r\nccc";
        cursor = 3U;
        selected = 2U;
        typed_cursor = 5U;
    }
    if (strcmp(name, "empty") == 0)
    {
        source = "";
        typed = "XY";
        cursor = 0U;
        typed_cursor = 2U;
    }
    if (strcmp(name, "unchanged") == 0)
    {
        typed = source;
        typed_cursor = 1U;
    }
    CHECK(Start(&f, source) == 0);
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f.workbench);
    if (strcmp(name, "pending-before-capture") == 0)
    {
        source = "pending";
        CHECK(Draft(&f, source) == UMI_STATUS_OK);
        wanted = UMI_STATUS_INVALID_STATE;
    }
    UmiUiDocumentViewSnapshot view;
    CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK);
    view.cursor_offset = cursor;
    view.selection_length = selected;
    view.read_only = strcmp(name, "capture-read-only") == 0;
    CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
    UmiDocumentEditPlan *capture = NULL;
    CHECK(UmiDocumentCoordinatorPrepareEdit(f.documents, f.id, &capture) == UMI_STATUS_OK);
    view.cursor_offset = typed_cursor;
    view.selection_length = 0U;
    view.read_only = strcmp(name, "read-only") == 0;
    view.dirty = 1;
    CHECK(UmiUiDocumentViewModelUpsertText(views, &view, typed, strlen(typed)) == UMI_STATUS_OK);
    if (strcmp(name, "read-only") == 0 || strcmp(name, "capture-read-only") == 0)
        wanted = UMI_STATUS_PERMISSION_DENIED;
    if (strcmp(name, "store-changed") == 0 || strcmp(name, "store-round-trip") == 0)
    {
        CHECK(umi_document_store_replace_text(f.store, f.id, "external", 8U) == UMI_STATUS_OK);
        if (strcmp(name, "store-round-trip") == 0)
            CHECK(umi_document_store_replace_text(f.store, f.id, source, strlen(source)) == UMI_STATUS_OK);
        wanted = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(name, "view-identity") == 0 || strcmp(name, "uri-changed") == 0)
    {
        if (strcmp(name, "view-identity") == 0)
            (void)snprintf(view.document_id, sizeof(view.document_id), "different");
        else
            (void)snprintf(view.uri, sizeof(view.uri), "untitled:///different.c");
        CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
        wanted = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(name, "invalid-selection") == 0)
    {
        view.cursor_offset = strlen(typed) + 1U;
        CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    UmiDocumentCoordinator *owner = f.documents;
    if (strcmp(name, "null-owner") == 0)
    {
        owner = NULL;
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(name, "null-capture") == 0)
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(name, "other-owner") == 0)
    {
        CHECK(Start(&other, "other") == 0);
        owner = other.documents;
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    UmiDocumentId active = f.id;
    if (strcmp(name, "other-tab") == 0)
    {
        CHECK(umi_document_coordinator_new(f.documents, "other.c", NULL, 0U) == UMI_STATUS_OK);
        UmiDocumentWorkingCopySnapshot state;
        CHECK(umi_document_coordinator_active_snapshot(f.documents, &state) == UMI_STATUS_OK);
        active = state.document_id;
    }
    if (strcmp(name, "closed") == 0)
    {
        CHECK(UmiDocumentCoordinatorClose(f.documents, f.id, 1) == UMI_STATUS_OK);
        wanted = UMI_STATUS_NOT_FOUND;
    }
    UmiDocumentSnapshot stored_before, stored_after;
    int exists = strcmp(name, "closed") != 0;
    if (exists)
        CHECK(umi_document_store_snapshot(f.store, f.id, &stored_before) == UMI_STATUS_OK);
    CHECK(UmiDocumentCoordinatorCommitTyping(owner, strcmp(name, "null-capture") == 0 ? NULL : capture) ==
          wanted);
    if (exists)
    {
        CHECK(ExpectText(&f, typed) == 0 &&
              umi_document_store_snapshot(f.store, f.id, &stored_after) == UMI_STATUS_OK);
        if (wanted != UMI_STATUS_OK || strcmp(name, "unchanged") == 0)
            CHECK(stored_before.revision == stored_after.revision);
        else
        {
            CHECK(UmiDocumentCoordinatorUndo(f.documents, f.id) == UMI_STATUS_OK &&
                  ExpectText(&f, source) == 0);
            CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK &&
                  view.cursor_offset == cursor && view.selection_length == selected);
            CHECK(UmiDocumentCoordinatorRedo(f.documents, f.id) == UMI_STATUS_OK &&
                  ExpectText(&f, typed) == 0);
            CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK &&
                  view.cursor_offset == typed_cursor && view.selection_length == 0U);
        }
        if (wanted == UMI_STATUS_OK)
        {
            UmiDocumentWorkingCopySnapshot state;
            CHECK(umi_document_coordinator_active_snapshot(f.documents, &state) == UMI_STATUS_OK &&
                  state.document_id == active);
        }
    }
    if (strcmp(name, "consumed") == 0)
        CHECK(UmiDocumentCoordinatorCommitTyping(f.documents, capture) == UMI_STATUS_INVALID_STATE);
    UmiDocumentEditPlanDestroy(capture);
    if (other.documents != NULL)
        Stop(&other);
    Stop(&f);
    return 0;
}
