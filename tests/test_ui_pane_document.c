/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ui_pane_document.c
 *
 * PURPOSE:
 *   Verify pane placement and document-tab state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(void)
{
    UmiUiPaneModel *panes = NULL;
    UmiUiDocumentViewModel *documents = NULL;
    UmiUiPaneSnapshot pane = {0};
    UmiUiDocumentViewSnapshot document = {0};
    assert(umi_ui_pane_model_create(&panes) == UMI_STATUS_OK);
    assert(umi_ui_document_view_model_create(&documents) == UMI_STATUS_OK);
    (void)snprintf(pane.pane_id, sizeof(pane.pane_id), "%s", "studio.explorer");
    (void)snprintf(pane.title, sizeof(pane.title), "%s", "Explorer");
    pane.placement = UMI_UI_PLACEMENT_LEFT; pane.visible = 1;
    assert(umi_ui_pane_model_upsert(panes, &pane) == UMI_STATUS_OK);
    (void)snprintf(document.view_id, sizeof(document.view_id), "%s", "view.readme");
    (void)snprintf(document.document_id, sizeof(document.document_id), "%s", "doc.readme");
    (void)snprintf(document.title, sizeof(document.title), "%s", "README.md");
    document.active = 1;
    assert(umi_ui_document_view_model_upsert(documents, &document) == UMI_STATUS_OK);
    assert(umi_ui_pane_model_count(panes) == 1U);
    assert(umi_ui_document_view_model_count(documents) == 1U);
    umi_ui_document_view_model_destroy(documents);
    umi_ui_pane_model_destroy(panes);
    return EXIT_SUCCESS;
}
