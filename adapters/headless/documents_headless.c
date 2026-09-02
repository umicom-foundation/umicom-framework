/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/headless/documents_headless.c
 *
 * PURPOSE:
 *   Render document tabs and associated editor state for deterministic headless tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "headless_internal.h"

/*
 * Provide the headless render documents operation used by this module and its client
 * applications.
 */
UmiStatus umi_headless_render_documents(UmiUiHeadlessAdapter *adapter,
                                        UmiUiWorkbench *workbench)
{
    UmiUiDocumentViewModel *documents = umi_ui_workbench_documents(workbench);
    size_t index;
    UmiStatus status = umi_headless_append(adapter, "[documents]\n");
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; status == UMI_STATUS_OK && index < umi_ui_document_view_model_count(documents); ++index) {
        UmiUiDocumentViewSnapshot document;
        status = umi_ui_document_view_model_at(documents, index, &document);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            status = umi_headless_append(adapter, "%s|%s|dirty=%d|active=%d\n",
                document.view_id, document.title, document.dirty, document.active);
            adapter->document_count += 1U;
        }
    }
    return status;
}
