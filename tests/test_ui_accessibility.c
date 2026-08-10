/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ui_accessibility.c
 *
 * PURPOSE:
 *   Verify accessible node metadata and state.
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
    UmiUiAccessibilityModel *model = NULL;
    UmiUiAccessibilitySnapshot node = {0};
    UmiUiAccessibilitySnapshot found;
    assert(umi_ui_accessibility_model_create(&model) == UMI_STATUS_OK);
    (void)snprintf(node.node_id, sizeof(node.node_id), "%s", "pane.explorer");
    (void)snprintf(node.role_name, sizeof(node.role_name), "%s", "tree");
    (void)snprintf(node.accessible_name, sizeof(node.accessible_name), "%s", "Project Explorer");
    node.enabled = 1;
    assert(umi_ui_accessibility_model_upsert(model, &node) == UMI_STATUS_OK);
    assert(umi_ui_accessibility_model_find(model, node.node_id, &found) == UMI_STATUS_OK);
    assert(strcmp(found.accessible_name, "Project Explorer") == 0);
    umi_ui_accessibility_model_destroy(model);
    return EXIT_SUCCESS;
}
