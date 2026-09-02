/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ui_layout.c
 *
 * PURPOSE:
 *   Verify layout parent relationships and single-root validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiUiLayout *layout = NULL;
    UmiUiLayoutNode root_node = {0};
    UmiUiLayoutNode child = {0};
    char message[256];
    assert(umi_ui_layout_create(&layout) == UMI_STATUS_OK);
    (void)snprintf(root_node.node_id, sizeof(root_node.node_id), "%s", "layout.root");
    root_node.kind = UMI_UI_LAYOUT_ROOT;
    assert(umi_ui_layout_upsert(layout, &root_node) == UMI_STATUS_OK);
    (void)snprintf(child.node_id, sizeof(child.node_id), "%s", "layout.documents");
    (void)snprintf(child.parent_id, sizeof(child.parent_id), "%s", "layout.root");
    (void)snprintf(child.target_id, sizeof(child.target_id), "%s", "documents");
    child.kind = UMI_UI_LAYOUT_DOCUMENTS; child.ratio = 1.0;
    assert(umi_ui_layout_upsert(layout, &child) == UMI_STATUS_OK);
    assert(umi_ui_layout_validate(layout, message, sizeof(message)) == UMI_STATUS_OK);
    assert(strstr(message, "valid") != NULL);
    umi_ui_layout_destroy(layout);
    return EXIT_SUCCESS;
}
