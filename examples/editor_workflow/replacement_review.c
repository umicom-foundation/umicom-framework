/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/editor_workflow/replacement_review.c
 * PURPOSE: Teach capture, review, apply and Undo using one in-memory draft.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/document.h"
#include <stdio.h>
#include <string.h>

/* This console lesson deliberately applies its own fixed example after
 * printing both versions. A graphical application waits for user acceptance. */
int main(void)
{
    UmiCommandRegistry *commands = NULL;
    UmiUiWorkbench *workbench = NULL;
    UmiDocumentStore *store = NULL;
    UmiDocumentCoordinator *documents = NULL;
    UmiDocumentReplacementPlan *plan = NULL;
    UmiStatus status = UMI_STATUS_OK;
    char viewId[UMI_UI_ID_CAPACITY];
    const char *draft = "note one\nnote two\n";
    UmiDocumentWorkingCopySnapshot snapshot;
    UmiUiDocumentViewSnapshot view;
#define TRY(call) do { status = (call); if (status != UMI_STATUS_OK) goto cleanup; } while (0)
    TRY(umi_command_registry_create(&commands));
    TRY(umi_ui_workbench_create("lesson.replacement", commands, &workbench));
    TRY(umi_document_store_create(&store));
    TRY(umi_document_coordinator_create(store, workbench, NULL, &documents));
    TRY(umi_document_coordinator_new(documents, "Notes.txt", viewId, sizeof viewId));
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(workbench);
    TRY(umi_ui_document_view_model_find(views, viewId, &view));
    view.dirty = 1;
    TRY(UmiUiDocumentViewModelUpsertText(views, &view, draft, strlen(draft)));
    TRY(umi_document_coordinator_active_snapshot(documents, &snapshot));
    TRY(UmiDocumentCoordinatorPrepareReplacement(documents, snapshot.document_id, "note", "entry", &plan));
    UmiDocumentReplacementSummary summary;
    const char *before = NULL, *after = NULL;
    size_t beforeBytes = 0U, afterBytes = 0U;
    TRY(UmiDocumentReplacementPlanSummary(plan, &summary));
    TRY(UmiDocumentReplacementPlanTexts(plan, &before, &beforeBytes, &after, &afterBytes));
    printf("Review %zu replacements in %s\nBefore (%zu bytes):\n%sAfter (%zu bytes):\n%s",
        summary.match_count, summary.display_name, beforeBytes, before, afterBytes, after);
    if (summary.match_count != 2U || strcmp(after, "entry one\nentry two\n") != 0) {
        status = UMI_STATUS_INVALID_STATE; goto cleanup;
    }
    TRY(UmiDocumentCoordinatorApplyReplacement(documents, plan, NULL));
    TRY(umi_document_coordinator_undo(documents));
    char *restored = NULL; size_t bytes = 0U;
    TRY(UmiUiDocumentViewModelCopyText(views, viewId, &restored, &bytes));
    int correct = bytes == strlen(draft) && strcmp(restored, draft) == 0;
    UmiUiDocumentViewModelFreeText(restored);
    if (!correct) { status = UMI_STATUS_INVALID_STATE; goto cleanup; }
    puts("Applied the reviewed draft, then restored it with Undo. No file was saved.");
cleanup:
    UmiDocumentReplacementPlanDestroy(plan);
    umi_document_coordinator_destroy(documents);
    umi_document_store_destroy(store);
    umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands);
    if (status != UMI_STATUS_OK) fprintf(stderr, "Replacement lesson: %s\n", umi_status_text(status));
    return status == UMI_STATUS_OK ? 0 : 1;
#undef TRY
}
