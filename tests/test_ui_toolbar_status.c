/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ui_toolbar_status.c
 *
 * PURPOSE:
 *   Verify toolbar contributions and visible status items.
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
    UmiUiToolbarModel *toolbar = NULL;
    UmiUiStatusModel *status = NULL;
    UmiUiToolbarSnapshot tool = {0};
    UmiUiStatusSnapshot item = {0};
    assert(umi_ui_toolbar_model_create(&toolbar) == UMI_STATUS_OK);
    assert(umi_ui_status_model_create(&status) == UMI_STATUS_OK);
    (void)snprintf(tool.item_id, sizeof(tool.item_id), "%s", "toolbar.build");
    (void)snprintf(tool.toolbar_id, sizeof(tool.toolbar_id), "%s", "studio.main");
    (void)snprintf(tool.action_id, sizeof(tool.action_id), "%s", "studio.build");
    assert(umi_ui_toolbar_model_upsert(toolbar, &tool) == UMI_STATUS_OK);
    (void)snprintf(item.item_id, sizeof(item.item_id), "%s", "status.workspace");
    (void)snprintf(item.text, sizeof(item.text), "%s", "Workspace ready");
    item.visible = 1;
    assert(umi_ui_status_model_upsert(status, &item) == UMI_STATUS_OK);
    assert(umi_ui_toolbar_model_count(toolbar) == 1U);
    assert(umi_ui_status_model_count(status) == 1U);
    umi_ui_status_model_destroy(status);
    umi_ui_toolbar_model_destroy(toolbar);
    return EXIT_SUCCESS;
}
