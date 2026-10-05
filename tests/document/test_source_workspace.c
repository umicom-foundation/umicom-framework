/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_source_workspace.c
 * PURPOSE: Exercise immutable source dependencies, identity checks and permission boundaries with real drafts.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "source_workspace_fixture.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *known[] = {
        "capture",      "order",         "empty-source",   "unicode",       "owned",
        "read-only",    "all-read-only", "duplicate-id",   "duplicate-uri", "missing-id",
        "zero-id",      "empty-uri",     "invalid-source", "source-change", "dependency-change",
        "changed-back", "caret",         "selection",      "language",      "permission",
        "uri",          "saved",         "external",       "closed",        "other-owner",
        "active-tab",   "presentation",  "new-document",   "maximum",       "count-limit",
        "arguments"};
    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    ReplacementFixture first = {0}, second = {0};
    const char *text = strcmp(mode, "empty-source") == 0 ? ""
                       : strcmp(mode, "unicode") == 0    ? "caf\xc3\xa9\r\nnext"
                                                         : "source";
    CHECK(Start(&first, text) == 0 && SourceFixtureAdd(&first, &second, "dependency.h", "dependency") == 0);
    UmiUiDocumentViewSnapshot first_view, second_view;
    CHECK(SourceFixtureView(&first, &first_view) == 0 && SourceFixtureView(&second, &second_view) == 0);
    UmiDocumentId ids[UMI_DOCUMENT_SOURCE_WORKSPACE_MAXIMUM] = {first.id, second.id};
    size_t count = 2U, index = 0U;
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(mode, "order") == 0)
    {
        ids[0] = second.id;
        ids[1] = first.id;
        index = 1U;
    }
    if (strcmp(mode, "read-only") == 0 || strcmp(mode, "all-read-only") == 0)
    {
        second_view.read_only = 1;
        CHECK(SourceFixturePut(&second, &second_view) == 0);
    }
    if (strcmp(mode, "all-read-only") == 0)
    {
        first_view.read_only = 1;
        CHECK(SourceFixturePut(&first, &first_view) == 0);
    }
    if (strcmp(mode, "duplicate-id") == 0)
    {
        ids[1] = ids[0];
        expected = UMI_STATUS_ALREADY_EXISTS;
    }
    if (strcmp(mode, "duplicate-uri") == 0)
    {
        strcpy(second_view.uri, first_view.uri);
        CHECK(SourceFixturePut(&second, &second_view) == 0);
        expected = UMI_STATUS_ALREADY_EXISTS;
    }
    if (strcmp(mode, "missing-id") == 0)
    {
        ids[1] = UINT64_MAX;
        expected = UMI_STATUS_NOT_FOUND;
    }
    if (strcmp(mode, "zero-id") == 0)
    {
        ids[1] = 0U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "empty-uri") == 0)
    {
        second_view.uri[0] = '\0';
        CHECK(SourceFixturePut(&second, &second_view) == 0);
        expected = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "invalid-source") == 0)
    {
        CHECK(Draft(&second, "\xc0\xaf") == UMI_STATUS_OK);
        expected = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "maximum") == 0)
    {
        for (size_t i = 2U; i < UMI_DOCUMENT_SOURCE_WORKSPACE_MAXIMUM; ++i)
        {
            ReplacementFixture added;
            char name[32];
            (void)snprintf(name, sizeof(name), "source-%zu.c", i);
            CHECK(SourceFixtureAdd(&first, &added, name, "small") == 0);
            ids[i] = added.id;
        }
        count = UMI_DOCUMENT_SOURCE_WORKSPACE_MAXIMUM;
    }
    if (strcmp(mode, "count-limit") == 0)
    {
        count = UMI_DOCUMENT_SOURCE_WORKSPACE_MAXIMUM + 1U;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    UmiDocumentSourceWorkspace *workspace = (UmiDocumentSourceWorkspace *)1;
    CHECK(UmiDocumentSourceWorkspaceCreate(first.documents, ids, count, &workspace) == expected);
    if (expected != UMI_STATUS_OK)
    {
        CHECK(workspace == NULL);
        goto cleanup;
    }
    CHECK(UmiDocumentSourceWorkspaceCount(workspace) == count &&
          UmiDocumentSourceWorkspaceCheck(first.documents, workspace) == UMI_STATUS_OK);
    UmiDocumentSourceRequestSummary document;
    const char *captured = NULL;
    size_t bytes = 0U, lookup = SIZE_MAX;
    CHECK(UmiDocumentSourceWorkspaceAt(workspace, index, &document) == UMI_STATUS_OK &&
          document.document_id == first.id);
    CHECK(UmiDocumentSourceWorkspaceRead(workspace, index, &captured, &bytes) == UMI_STATUS_OK &&
          bytes == strlen(text) && strcmp(captured, text) == 0);
    CHECK(UmiDocumentSourceWorkspaceFind(workspace, first_view.uri, &lookup) == UMI_STATUS_OK &&
          lookup == index);
    expected = UMI_STATUS_OK;
    if (strcmp(mode, "owned") == 0 || strcmp(mode, "source-change") == 0)
    {
        CHECK(Draft(&first, "later") == UMI_STATUS_OK);
        expected = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "dependency-change") == 0 || strcmp(mode, "changed-back") == 0)
    {
        CHECK(Draft(&second, "later") == UMI_STATUS_OK);
        if (strcmp(mode, "changed-back") == 0)
            CHECK(Draft(&second, "dependency") == UMI_STATUS_OK);
        expected = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "caret") == 0 || strcmp(mode, "selection") == 0 || strcmp(mode, "language") == 0 ||
        strcmp(mode, "permission") == 0 || strcmp(mode, "uri") == 0 || strcmp(mode, "presentation") == 0)
    {
        if (strcmp(mode, "caret") == 0)
            second_view.cursor_offset = 1U;
        if (strcmp(mode, "selection") == 0)
            second_view.selection_length = 2U;
        if (strcmp(mode, "language") == 0)
            strcpy(second_view.language_id, "changed");
        if (strcmp(mode, "permission") == 0)
            second_view.read_only = 1;
        if (strcmp(mode, "uri") == 0)
            strcpy(second_view.uri, "untitled:changed");
        if (strcmp(mode, "presentation") == 0)
        {
            second_view.pinned = 1;
            second_view.word_wrap = 1;
            strcpy(second_view.title, "Presentation only");
        }
        CHECK(SourceFixturePut(&second, &second_view) == 0);
        if (strcmp(mode, "presentation") != 0)
            expected = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "saved") == 0)
    {
        CHECK(umi_document_store_mark_saved_as(first.store, second.id, "saved.h") == UMI_STATUS_OK);
        expected = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "external") == 0)
    {
        CHECK(umi_document_store_mark_external_change(first.store, second.id, 1) == UMI_STATUS_OK);
        expected = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "closed") == 0)
    {
        CHECK(umi_document_coordinator_close_active(first.documents, 1) == UMI_STATUS_OK);
        expected = UMI_STATUS_NOT_FOUND;
    }
    if (strcmp(mode, "active-tab") == 0)
        CHECK(umi_ui_document_view_model_activate(umi_ui_workbench_documents(first.workbench),
                                                  first.viewId) == UMI_STATUS_OK);
    if (strcmp(mode, "new-document") == 0)
        CHECK(umi_document_coordinator_new(first.documents, "later.c", NULL, 0U) == UMI_STATUS_OK);
    if (strcmp(mode, "other-owner") == 0)
    {
        ReplacementFixture other = {0};
        CHECK(Start(&other, "source") == 0);
        CHECK(UmiDocumentSourceWorkspaceCheck(other.documents, workspace) == UMI_STATUS_INVALID_STATE);
        Stop(&other);
    }
    CHECK(UmiDocumentSourceWorkspaceCheck(first.documents, workspace) == expected);
    CHECK(strcmp(captured, text) == 0 && UmiDocumentSourceWorkspaceCount(workspace) == count);
    if (strcmp(mode, "arguments") == 0)
    {
        UmiDocumentSourceWorkspace *bad = workspace;
        CHECK(UmiDocumentSourceWorkspaceCreate(NULL, ids, 2U, &bad) == UMI_STATUS_INVALID_ARGUMENT &&
              bad == NULL);
        CHECK(UmiDocumentSourceWorkspaceCreate(first.documents, NULL, 2U, &bad) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentSourceWorkspaceCreate(first.documents, ids, 0U, &bad) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentSourceWorkspaceCreate(first.documents, ids, 2U, NULL) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentSourceWorkspaceCount(NULL) == 0U);
        CHECK(UmiDocumentSourceWorkspaceAt(workspace, count, &document) == UMI_STATUS_NOT_FOUND);
        CHECK(UmiDocumentSourceWorkspaceRead(workspace, count, &captured, &bytes) == UMI_STATUS_NOT_FOUND &&
              captured == NULL && bytes == 0U);
        CHECK(UmiDocumentSourceWorkspaceRead(NULL, 0U, &captured, &bytes) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentSourceWorkspaceFind(workspace, "missing", &lookup) == UMI_STATUS_NOT_FOUND &&
              lookup == SIZE_MAX);
        CHECK(UmiDocumentSourceWorkspaceFind(workspace, "", &lookup) == UMI_STATUS_INVALID_ARGUMENT &&
              lookup == SIZE_MAX);
        CHECK(UmiDocumentSourceWorkspaceCheck(NULL, workspace) == UMI_STATUS_INVALID_ARGUMENT);
        UmiDocumentSourceWorkspaceDestroy(NULL);
    }
cleanup:
    UmiDocumentSourceWorkspaceDestroy(workspace);
    Stop(&first);
    return 0;
}
