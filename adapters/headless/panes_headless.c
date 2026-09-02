/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/headless/panes_headless.c
 *
 * PURPOSE:
 *   Render pane metadata and any registered toolkit-neutral view presentation
 *   for deterministic headless inspection.
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
 * Provide the headless render panes operation used by this module and its client
 * applications.
 */
UmiStatus umi_headless_render_panes(UmiUiHeadlessAdapter *adapter,
                                    UmiUiWorkbench *workbench)
{
    UmiUiPaneModel *panes = umi_ui_workbench_panes(workbench);
    size_t index;
    UmiStatus status = umi_headless_append(adapter, "[panes]\n");

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         status == UMI_STATUS_OK &&
         index < umi_ui_pane_model_count(panes);
         ++index) {
        UmiUiPaneSnapshot pane;

        status = umi_ui_pane_model_at(panes, index, &pane);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            status = umi_headless_append(
                adapter,
                "%s|%s|%s|visible=%d\n",
                pane.pane_id,
                pane.title,
                umi_ui_placement_text(pane.placement),
                pane.visible);
            adapter->pane_count += 1U;
        }

        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK && pane.visible) {
            status = umi_headless_render_pane_view(
                adapter, workbench, &pane);
        }
    }
    return status;
}
