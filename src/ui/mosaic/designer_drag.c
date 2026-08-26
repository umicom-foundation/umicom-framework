/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/mosaic/designer_drag.c
 *
 * PURPOSE:
 *   Define toolkit-neutral designer drag contracts for the Framework-owned workbench mosaic platform.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/mosaic/designer_drag.h"
#include <string.h>

void umi_ui_mosaic_designer_drag_init(UmiUiMosaicDesignerDrag *value) {
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->revision = 1U;
    value->mode = UMI_UI_MOSAIC_EDIT_LOCKED;
    value->valid = true;
}

UmiStatus umi_ui_mosaic_designer_drag_bind(UmiUiMosaicDesignerDrag *value, const char *workspace_id, const char *active_id) {
    UmiStatus status;
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_mosaic_copy_text(value->workspace_id, sizeof(value->workspace_id), workspace_id);
    if (status != UMI_STATUS_OK) return status;
    return umi_ui_mosaic_copy_text(value->active_id, sizeof(value->active_id), active_id);
}

UmiStatus umi_ui_mosaic_designer_drag_validate(const UmiUiMosaicDesignerDrag *value) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!value->valid || !umi_ui_mosaic_id_is_valid(value->workspace_id) || !umi_ui_mosaic_id_is_valid(value->active_id)) return UMI_STATUS_INVALID_STATE;
    if (value->selection_count > UMI_UI_MOSAIC_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_mosaic_designer_drag_advance(UmiUiMosaicDesignerDrag *value) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (value->mode != UMI_UI_MOSAIC_EDIT_UNLOCKED) return UMI_STATUS_PERMISSION_DENIED;
    if (value->revision == UINT32_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    value->revision += 1U;
    return UMI_STATUS_OK;
}
