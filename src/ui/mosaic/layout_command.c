/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/mosaic/layout_command.c
 *
 * PURPOSE:
 *   Define toolkit-neutral layout command contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/mosaic/layout_command.h"
#include <string.h>

void umi_ui_mosaic_layout_command_init(UmiUiMosaicLayoutCommand *value) {
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->enabled = true;
}

UmiStatus umi_ui_mosaic_layout_command_set(UmiUiMosaicLayoutCommand *value, const char *id, const char *label) {
    UmiStatus status;
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_mosaic_copy_text(value->id, sizeof(value->id), id);
    if (status != UMI_STATUS_OK) return status;
    return umi_ui_mosaic_copy_text(value->label, sizeof(value->label), label);
}

UmiStatus umi_ui_mosaic_layout_command_validate(const UmiUiMosaicLayoutCommand *value) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_ui_mosaic_id_is_valid(value->id) || value->label[0] == '\0') return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

int umi_ui_mosaic_layout_command_can_execute(const UmiUiMosaicLayoutCommand *value, UmiUiMosaicEditMode mode) {
    if (value == NULL || !value->enabled) return 0;
    if (value->requires_edit_mode && mode != UMI_UI_MOSAIC_EDIT_UNLOCKED) return 0;
    return 1;
}
