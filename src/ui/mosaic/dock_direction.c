/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/mosaic/dock_direction.c
 *
 * PURPOSE:
 *   Define toolkit-neutral dock direction contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/mosaic/dock_direction.h"
#include <string.h>

void umi_ui_mosaic_dock_direction_init(UmiUiMosaicDockDirection *value) {
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->zone = UMI_UI_MOSAIC_DOCK_CENTRE;
    value->allowed = true;
}

UmiStatus umi_ui_mosaic_dock_direction_set(UmiUiMosaicDockDirection *value, const char *source_id, const char *target_id, UmiUiMosaicDockZone zone) {
    UmiStatus status;
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_mosaic_copy_text(value->source_id, sizeof(value->source_id), source_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_mosaic_copy_text(value->target_id, sizeof(value->target_id), target_id);
    if (status != UMI_STATUS_OK) return status;
    value->zone = zone;
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_mosaic_dock_direction_validate(const UmiUiMosaicDockDirection *value) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!value->allowed || !umi_ui_mosaic_id_is_valid(value->source_id) || !umi_ui_mosaic_id_is_valid(value->target_id)) return UMI_STATUS_INVALID_STATE;
    if (strcmp(value->source_id, value->target_id) == 0) return UMI_STATUS_INVALID_STATE;
    if (value->zone < UMI_UI_MOSAIC_DOCK_LEFT || value->zone > UMI_UI_MOSAIC_DOCK_FLOAT) return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

int umi_ui_mosaic_dock_direction_is_centre(const UmiUiMosaicDockDirection *value) {
    return value != NULL && value->zone == UMI_UI_MOSAIC_DOCK_CENTRE;
}
