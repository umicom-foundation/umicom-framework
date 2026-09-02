/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/mosaic/mosaic_cell.c
 *
 * PURPOSE:
 *   Define toolkit-neutral mosaic cell contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/mosaic/mosaic_cell.h"
#include <string.h>

/*
 * Initialise ui mosaic mosaic cell from caller-provided values so later operations receive
 * a known state.
 */
void umi_ui_mosaic_mosaic_cell_init(UmiUiMosaicMosaicCell *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->application = UMI_UI_MOSAIC_APP_FRAMEWORK;
    value->row_span = 1U;
    value->column_span = 1U;
    value->active = true;
}

/*
 * Provide the ui mosaic mosaic cell place operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_mosaic_mosaic_cell_place(UmiUiMosaicMosaicCell *value, const char *id, const char *panel_id, UmiUiMosaicApplication application, uint16_t row, uint16_t column) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_mosaic_copy_text(value->id, sizeof(value->id), id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_mosaic_copy_text(value->panel_id, sizeof(value->panel_id), panel_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->application = application;
    value->row = row;
    value->column = column;
    return UMI_STATUS_OK;
}

/*
 * Check that ui mosaic mosaic cell satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_ui_mosaic_mosaic_cell_validate(const UmiUiMosaicMosaicCell *value) {
    uint32_t end_row; uint32_t end_column;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!value->active || !umi_ui_mosaic_id_is_valid(value->id) || !umi_ui_mosaic_id_is_valid(value->panel_id)) return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (value->application < UMI_UI_MOSAIC_APP_FRAMEWORK || value->application > UMI_UI_MOSAIC_APP_OS) return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (value->row_span == 0U || value->column_span == 0U) return UMI_STATUS_INVALID_STATE;
    end_row = (uint32_t)value->row + (uint32_t)value->row_span;
    end_column = (uint32_t)value->column + (uint32_t)value->column_span;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (end_row > UMI_UI_MOSAIC_MAX_CELLS || end_column > UMI_UI_MOSAIC_MAX_CELLS) return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}

/*
 * Provide the ui mosaic mosaic cell area operation used by this module and its client
 * applications.
 */
uint32_t umi_ui_mosaic_mosaic_cell_area(const UmiUiMosaicMosaicCell *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return 0U;
    return (uint32_t)value->row_span * (uint32_t)value->column_span;
}
