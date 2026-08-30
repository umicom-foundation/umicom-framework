/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/mosaic/floating_panel.c
 *
 * PURPOSE:
 *   Define toolkit-neutral floating panel contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/mosaic/floating_panel.h"
#include <string.h>

void umi_ui_mosaic_floating_panel_init(UmiUiMosaicFloatingPanel *value) {
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->revision = 1U;
}

UmiStatus umi_ui_mosaic_floating_panel_set(UmiUiMosaicFloatingPanel *value, const char *id, const char *name) {
    UmiStatus status;
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_mosaic_copy_text(value->id, sizeof(value->id), id);
    if (status != UMI_STATUS_OK) return status;
    return umi_ui_mosaic_copy_text(value->name, sizeof(value->name), name);
}

UmiStatus umi_ui_mosaic_floating_panel_validate(const UmiUiMosaicFloatingPanel *value) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_ui_mosaic_id_is_valid(value->id) || value->name[0] == '\0' || value->revision == 0U) return UMI_STATUS_INVALID_STATE;
    if (value->item_count > UMI_UI_MOSAIC_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_mosaic_floating_panel_touch(UmiUiMosaicFloatingPanel *value) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (value->locked) return UMI_STATUS_PERMISSION_DENIED;
    if (value->revision == UINT32_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    value->revision += 1U;
    return UMI_STATUS_OK;
}
