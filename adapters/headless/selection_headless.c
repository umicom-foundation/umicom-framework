/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/headless/selection_headless.c
 *
 * PURPOSE:
 *   Render selected workbench item identifiers for deterministic frontend tests.
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
 * Provide the headless render selection operation used by this module and its client
 * applications.
 */
UmiStatus umi_headless_render_selection(UmiUiHeadlessAdapter *adapter,
                                        UmiUiWorkbench *workbench)
{
    UmiUiSelectionModel *selection = umi_ui_workbench_selection(workbench);
    size_t index;
    UmiStatus status = umi_headless_append(adapter, "[selection]\n");
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; status == UMI_STATUS_OK && index < umi_ui_selection_count(selection); ++index) {
        char item_id[UMI_UI_ID_CAPACITY];
        status = umi_ui_selection_at(selection, index, item_id, sizeof(item_id));
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) status = umi_headless_append(adapter, "%s\n", item_id);
    }
    return status;
}
