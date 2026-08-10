/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/headless/panes_headless.c
 *
 * PURPOSE:
 *   Render pane identity, placement, visibility and title for headless workbench tests.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "headless_internal.h"

UmiStatus umi_headless_render_panes(UmiUiHeadlessAdapter *adapter,
                                    UmiUiWorkbench *workbench)
{
    UmiUiPaneModel *panes = umi_ui_workbench_panes(workbench);
    size_t index;
    UmiStatus status = umi_headless_append(adapter, "[panes]\n");
    for (index = 0U; status == UMI_STATUS_OK && index < umi_ui_pane_model_count(panes); ++index) {
        UmiUiPaneSnapshot pane;
        status = umi_ui_pane_model_at(panes, index, &pane);
        if (status == UMI_STATUS_OK) {
            status = umi_headless_append(adapter, "%s|%s|%s|visible=%d\n",
                pane.pane_id, pane.title, umi_ui_placement_text(pane.placement), pane.visible);
            adapter->pane_count += 1U;
        }
    }
    return status;
}
