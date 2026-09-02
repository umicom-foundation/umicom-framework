/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/headless/status_headless.c
 *
 * PURPOSE:
 *   Render ordered status items for terminal and automated snapshot inspection.
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
 * Provide the headless render status operation used by this module and its client
 * applications.
 */
UmiStatus umi_headless_render_status(UmiUiHeadlessAdapter *adapter,
                                     UmiUiWorkbench *workbench)
{
    UmiUiStatusModel *model = umi_ui_workbench_status(workbench);
    size_t index;
    UmiStatus status = umi_headless_append(adapter, "[status]\n");
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; status == UMI_STATUS_OK && index < umi_ui_status_model_count(model); ++index) {
        UmiUiStatusSnapshot item;
        status = umi_ui_status_model_at(model, index, &item);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK && item.visible) {
            status = umi_headless_append(adapter, "%s|%s\n", item.item_id, item.text);
        }
    }
    return status;
}
