/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/mosaic/workbench_mosaic_service.c
 *
 * PURPOSE:
 *   Aggregate panel catalogue, layouts, perspectives, workspaces and context-link groups into one Framework service.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/mosaic/workbench_mosaic_service.h"
#include <string.h>

void umi_ui_mosaic_workbench_mosaic_service_init(UmiUiMosaicWorkbenchMosaicService *value) {
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->revision = 1U;
    value->mode = UMI_UI_MOSAIC_EDIT_LOCKED;
    value->valid = true;
}

UmiStatus umi_ui_mosaic_workbench_mosaic_service_bind(UmiUiMosaicWorkbenchMosaicService *value, const char *workspace_id, const char *active_id) {
    UmiStatus status;
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_mosaic_copy_text(value->workspace_id, sizeof(value->workspace_id), workspace_id);
    if (status != UMI_STATUS_OK) return status;
    return umi_ui_mosaic_copy_text(value->active_id, sizeof(value->active_id), active_id);
}

UmiStatus umi_ui_mosaic_workbench_mosaic_service_validate(const UmiUiMosaicWorkbenchMosaicService *value) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!value->valid || !umi_ui_mosaic_id_is_valid(value->workspace_id) || !umi_ui_mosaic_id_is_valid(value->active_id)) return UMI_STATUS_INVALID_STATE;
    if (value->selection_count > UMI_UI_MOSAIC_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_mosaic_workbench_mosaic_service_advance(UmiUiMosaicWorkbenchMosaicService *value) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (value->mode != UMI_UI_MOSAIC_EDIT_UNLOCKED) return UMI_STATUS_PERMISSION_DENIED;
    if (value->revision == UINT32_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    value->revision += 1U;
    return UMI_STATUS_OK;
}
