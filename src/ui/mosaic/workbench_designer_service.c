/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/mosaic/workbench_designer_service.c
 *
 * PURPOSE:
 *   Aggregate selection, drag, drop-preview and edit-session state for a renderer-neutral Layout Designer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/mosaic/workbench_designer_service.h"
#include <string.h>

/*
 * Initialise ui mosaic workbench designer service from caller-provided values so later
 * operations receive a known state.
 */
void umi_ui_mosaic_workbench_designer_service_init(UmiUiMosaicWorkbenchDesignerService *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->revision = 1U;
    value->mode = UMI_UI_MOSAIC_EDIT_LOCKED;
    value->valid = true;
}

/*
 * Provide the ui mosaic workbench designer service bind operation used by this module and
 * its client applications.
 */
UmiStatus umi_ui_mosaic_workbench_designer_service_bind(UmiUiMosaicWorkbenchDesignerService *value, const char *workspace_id, const char *active_id) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_mosaic_copy_text(value->workspace_id, sizeof(value->workspace_id), workspace_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_ui_mosaic_copy_text(value->active_id, sizeof(value->active_id), active_id);
}

/*
 * Check that ui mosaic workbench designer service satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_ui_mosaic_workbench_designer_service_validate(const UmiUiMosaicWorkbenchDesignerService *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!value->valid || !umi_ui_mosaic_id_is_valid(value->workspace_id) || !umi_ui_mosaic_id_is_valid(value->active_id)) return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (value->selection_count > UMI_UI_MOSAIC_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}

/*
 * Provide the ui mosaic workbench designer service advance operation used by this module
 * and its client applications.
 */
UmiStatus umi_ui_mosaic_workbench_designer_service_advance(UmiUiMosaicWorkbenchDesignerService *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (value->mode != UMI_UI_MOSAIC_EDIT_UNLOCKED) return UMI_STATUS_PERMISSION_DENIED;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (value->revision == UINT32_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    value->revision += 1U;
    return UMI_STATUS_OK;
}
