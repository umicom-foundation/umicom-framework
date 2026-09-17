/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/editor_workflow/complete_document.c
 *
 * PURPOSE:
 *   Edit an in-memory Notes source document beyond its metadata preview.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/document.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* This programme changes an unsaved document only. It never overwrites files. */
int main(void)
{
    UmiCommandRegistry *commands = NULL;
    UmiUiWorkbench *workbench = NULL;
    UmiDocumentStore *store = NULL;
    UmiDocumentCoordinator *documents = NULL;
    UmiUiDocumentViewSnapshot view = {0};
    char viewId[UMI_UI_ID_CAPACITY];
    char *source = NULL;
    char *restored = NULL;
    size_t restoredLength = 0U;
    size_t foundAt = 0U;
    int result = 1;
    const size_t prefixLength = 65536U;
    const char *body = "int noteCount = 3;\nint total = noteCount;\n";
    const size_t sourceLength = prefixLength + strlen(body);

    /* Give each service an explicit owner. Cleanup below reverses this order. */
    if (umi_command_registry_create(&commands) != UMI_STATUS_OK ||
        umi_ui_workbench_create("umicom.notes.editor", commands, &workbench) != UMI_STATUS_OK ||
        umi_document_store_create(&store) != UMI_STATUS_OK ||
        umi_document_coordinator_create(store, workbench, NULL, &documents) != UMI_STATUS_OK ||
        umi_document_coordinator_new(documents, "notes.c", viewId, sizeof viewId) != UMI_STATUS_OK)
        goto cleanup;

    /* Blank lines stand in for the earlier part of a larger source file.
     * The names we will edit are deliberately after the first 64 KiB. */
    source = malloc(sourceLength + 1U);
    if (source == NULL) goto cleanup;
    memset(source, ' ', prefixLength);
    for (size_t index = 63U; index < prefixLength; index += 64U) source[index] = '\n';
    memcpy(source + prefixLength, body, strlen(body) + 1U);

    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(workbench);
    if (umi_ui_document_view_model_find(views, viewId, &view) != UMI_STATUS_OK) goto cleanup;
    view.dirty = 1;
    if (UmiUiDocumentViewModelUpsertText(views, &view, source, sourceLength) != UMI_STATUS_OK)
        goto cleanup;

    /* Metadata tells us whether its small text preview is complete. */
    UmiUiDocumentTextInfo info;
    if (UmiUiDocumentViewModelTextInfo(views, viewId, &info) != UMI_STATUS_OK) goto cleanup;
    printf("Notes source: %zu bytes. Full text fits the preview: %s.\n",
        info.byte_count, info.preview_complete ? "yes" : "no");

    if (umi_document_coordinator_find(documents, "noteCount", &foundAt) != UMI_STATUS_OK)
        goto cleanup;
    printf("Found noteCount at byte %zu.\n", foundAt);
    size_t replacements = 0U;
    if (UmiDocumentCoordinatorReplaceAll(documents, "noteCount", "savedCount", &replacements) != UMI_STATUS_OK ||
        replacements != 2U || umi_document_coordinator_undo(documents) != UMI_STATUS_OK)
        goto cleanup;

    /* CopyText returns our own allocation, which stays valid independently
     * of the model. Always release it with the matching Framework function. */
    if (UmiUiDocumentViewModelCopyText(views, viewId, &restored, &restoredLength) != UMI_STATUS_OK)
        goto cleanup;
    if (foundAt != prefixLength + 4U || restoredLength != sourceLength ||
        memcmp(restored, source, sourceLength) != 0) goto cleanup;
    puts("Replaced two names, then restored the complete source with Undo.");
    result = 0;

cleanup:
    if (result != 0) fputs("The Notes editing example could not complete.\n", stderr);
    UmiUiDocumentViewModelFreeText(restored);
    free(source);
    umi_document_coordinator_destroy(documents);
    umi_document_store_destroy(store);
    umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands);
    return result;
}
