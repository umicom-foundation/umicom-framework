/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_completion_source_request.c
 * PURPOSE: Exercise completion planning, document review and Undo through their public boundaries.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"
#include "umicom/document/source_request.h"
#include "umicom/language_runtime/completion_preview.h"

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *known[] = {"apply", "cancel", "typing", "selection", "other-tab",
                           "undo",  "redo",   "import", "invalid",   "second-review"};
    int known_case = 0;
    for (size_t i = 0; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            known_case = 1;
    CHECK(known_case);
    ReplacementFixture fixture = {0};
    CHECK(Start(&fixture, "//\npu\n") == 0);
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(fixture.workbench);
    UmiUiDocumentViewSnapshot view;
    CHECK(umi_ui_document_view_model_find(views, fixture.viewId, &view) == UMI_STATUS_OK);
    view.cursor_offset = 5U;
    view.selection_length = 0U;
    CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
    UmiDocumentSourceRequest *request = NULL;
    CHECK(UmiDocumentSourceRequestCreate(fixture.documents, fixture.id, &request) == UMI_STATUS_OK);
    UmiDocumentSourceRequestSummary summary;
    CHECK(UmiDocumentSourceRequestInspect(request, &summary) == UMI_STATUS_OK);
    const char *source = NULL;
    size_t bytes = 0U;
    CHECK(UmiDocumentSourceRequestRead(request, &source, &bytes) == UMI_STATUS_OK);
    const char *json =
        strcmp(mode, "import") == 0
            ? "[{\"label\":\"puts\",\"additionalTextEdits\":[{\"range\":{\"start\":{\"line\":0,\"character\":"
              "0},\"end\":{\"line\":0,\"character\":0}},\"newText\":\"#include <stdio.h>\\n\"}]}]"
        : strcmp(mode, "invalid") == 0 ? "[{\"label\":\"puts\",\"insertTextFormat\":2}]"
                                       : "[{\"label\":\"puts\"}]";
    UmiLanguageCompletionCatalogue *catalogue = NULL;
    CHECK(UmiLanguageCompletionCatalogueCreate(json, strlen(json), NULL, &catalogue) == UMI_STATUS_OK);
    UmiLanguageCompletionPreview *preview = NULL;
    UmiStatus planned = UmiLanguageCompletionPreviewCreate(catalogue, 0U, summary.uri, "selected", source,
                                                           bytes, summary.cursor_offset, 3U, 5U,
                                                           UMI_LANGUAGE_COMPLETION_REPLACE, NULL, &preview);
    if (strcmp(mode, "invalid") == 0)
    {
        CHECK(planned == UMI_STATUS_NOT_IMPLEMENTED && preview == NULL);
        CHECK(ExpectText(&fixture, "//\npu\n") == 0);
        goto cleanup;
    }
    CHECK(planned == UMI_STATUS_OK);
    const char *proposed = NULL;
    size_t proposed_bytes = 0U, caret = 0U;
    CHECK(UmiLanguageCompletionPreviewRead(preview, &proposed, &proposed_bytes, &caret) == UMI_STATUS_OK);
    CHECK(UmiDocumentSourceRequestStage(request, summary.revision, proposed, proposed_bytes, caret) ==
          UMI_STATUS_OK);
    UmiLanguageCompletionPreviewDestroy(preview);
    preview = NULL;
    CHECK(UmiDocumentSourceRequestInspect(request, &summary) == UMI_STATUS_OK);
    CHECK(ExpectText(&fixture, "//\npu\n") == 0);
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(mode, "typing") == 0)
    {
        CHECK(Draft(&fixture, "//\npush\n") == UMI_STATUS_OK);
        expected = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "selection") == 0)
    {
        view.selection_length = 1U;
        view.cursor_offset = 4U;
        CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
        expected = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "second-review") == 0)
    {
        CHECK(UmiDocumentSourceRequestStage(request, summary.revision, "//\nother\n", 9U, 8U) ==
              UMI_STATUS_OK);
        expected = UMI_STATUS_BUSY;
    }
    char other[UMI_UI_ID_CAPACITY] = {0};
    if (strcmp(mode, "other-tab") == 0)
        CHECK(umi_document_coordinator_new(fixture.documents, "other.c", other, sizeof(other)) ==
              UMI_STATUS_OK);
    if (strcmp(mode, "cancel") == 0)
    {
        CHECK(ExpectText(&fixture, "//\npu\n") == 0);
        goto cleanup;
    }
    CHECK(UmiDocumentSourceRequestApply(fixture.documents, request, summary.revision, 1) == expected);
    if (expected == UMI_STATUS_OK)
    {
        const char *after = strcmp(mode, "import") == 0 ? "#include <stdio.h>\n//\nputs\n" : "//\nputs\n";
        CHECK(ExpectText(&fixture, after) == 0);
        CHECK(umi_ui_document_view_model_find(views, fixture.viewId, &view) == UMI_STATUS_OK);
        CHECK(view.cursor_offset == strlen(after) - 1U && view.selection_length == 0U);
        if (strcmp(mode, "other-tab") == 0)
        {
            UmiDocumentWorkingCopySnapshot active;
            CHECK(umi_document_coordinator_active_snapshot(fixture.documents, &active) == UMI_STATUS_OK);
            CHECK(strcmp(active.view_id, other) == 0);
        }
        if (strcmp(mode, "undo") == 0 || strcmp(mode, "redo") == 0)
        {
            CHECK(UmiDocumentCoordinatorUndo(fixture.documents, fixture.id) == UMI_STATUS_OK);
            CHECK(ExpectText(&fixture, "//\npu\n") == 0);
            if (strcmp(mode, "redo") == 0)
            {
                CHECK(UmiDocumentCoordinatorRedo(fixture.documents, fixture.id) == UMI_STATUS_OK);
                CHECK(ExpectText(&fixture, after) == 0);
            }
        }
    }
    else
        CHECK(ExpectText(&fixture, strcmp(mode, "typing") == 0 ? "//\npush\n" : "//\npu\n") == 0);
cleanup:
    UmiLanguageCompletionPreviewDestroy(preview);
    UmiLanguageCompletionCatalogueDestroy(catalogue);
    UmiDocumentSourceRequestDestroy(request);
    Stop(&fixture);
    return 0;
}
