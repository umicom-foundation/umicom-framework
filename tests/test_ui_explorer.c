/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ui_explorer.c
 *
 * PURPOSE:
 *   Exercise one Batch 23 unified-workbench contract through its public C API.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * These tests are intentionally small. They document expected behaviour without starting a graphical application.
 */

#include "umicom/umicom.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    UmiUiExplorerModel *model = NULL;
    UmiUiExplorerNode node = {0};
    UmiUiExplorerSearchResults results;
    UmiUiExplorerNode found;

    assert(umi_ui_explorer_model_create(&model) == UMI_STATUS_OK);

    (void)snprintf(node.node_id, sizeof(node.node_id), "%s", "file.workbench");
    (void)snprintf(node.name, sizeof(node.name), "%s", "workbench.c");
    (void)snprintf(node.path, sizeof(node.path), "%s",
                   "applications/studio/src/app/workbench.c");
    node.kind = UMI_UI_EXPLORER_FILE;

    assert(umi_ui_explorer_model_upsert(model, &node) == UMI_STATUS_OK);
    assert(umi_ui_explorer_model_select(model, node.node_id) == UMI_STATUS_OK);
    assert(umi_ui_explorer_search(model, "workb", &results) == UMI_STATUS_OK);
    assert(results.count == 1U);
    assert(umi_ui_explorer_model_find(model, node.node_id, &found) == UMI_STATUS_OK);
    assert(found.selected == 1);

    umi_ui_explorer_model_destroy(model);
    return 0;
}
