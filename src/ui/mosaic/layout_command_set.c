/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/mosaic/layout_command_set.c
 *
 * PURPOSE:
 *   Define toolkit-neutral layout command set contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/mosaic/layout_command_set.h"
#include <string.h>

/*
 * Initialise ui mosaic layout command set from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_mosaic_layout_command_set_init(UmiUiMosaicLayoutCommandSet *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->enabled = true;
}

/*
 * Copy ui mosaic layout command set into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_ui_mosaic_layout_command_set_set(UmiUiMosaicLayoutCommandSet *value, const char *id, const char *label) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_mosaic_copy_text(value->id, sizeof(value->id), id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_ui_mosaic_copy_text(value->label, sizeof(value->label), label);
}

/*
 * Check that ui mosaic layout command set satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_ui_mosaic_layout_command_set_validate(const UmiUiMosaicLayoutCommandSet *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_ui_mosaic_id_is_valid(value->id) || value->label[0] == '\0') return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

/*
 * Perform ui mosaic layout command set can through the module contract so client
 * applications do not duplicate its policy.
 */
int umi_ui_mosaic_layout_command_set_can_execute(const UmiUiMosaicLayoutCommandSet *value, UmiUiMosaicEditMode mode) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || !value->enabled) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (value->requires_edit_mode && mode != UMI_UI_MOSAIC_EDIT_UNLOCKED) return 0;
    return 1;
}
