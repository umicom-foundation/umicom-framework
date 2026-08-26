/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/mosaic/application_panel_pack.c
 *
 * PURPOSE:
 *   Define toolkit-neutral application panel pack contracts for the Framework-owned workbench mosaic platform.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/mosaic/application_panel_pack.h"
#include <string.h>

void umi_ui_mosaic_application_panel_pack_init(UmiUiMosaicApplicationPanelPack *value) {
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->application = UMI_UI_MOSAIC_APP_FRAMEWORK;
    value->row_span = 1U;
    value->column_span = 1U;
    value->active = true;
}

UmiStatus umi_ui_mosaic_application_panel_pack_place(UmiUiMosaicApplicationPanelPack *value, const char *id, const char *panel_id, UmiUiMosaicApplication application, uint16_t row, uint16_t column) {
    UmiStatus status;
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_mosaic_copy_text(value->id, sizeof(value->id), id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_mosaic_copy_text(value->panel_id, sizeof(value->panel_id), panel_id);
    if (status != UMI_STATUS_OK) return status;
    value->application = application;
    value->row = row;
    value->column = column;
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_mosaic_application_panel_pack_validate(const UmiUiMosaicApplicationPanelPack *value) {
    uint32_t end_row; uint32_t end_column;
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!value->active || !umi_ui_mosaic_id_is_valid(value->id) || !umi_ui_mosaic_id_is_valid(value->panel_id)) return UMI_STATUS_INVALID_STATE;
    if (value->application < UMI_UI_MOSAIC_APP_FRAMEWORK || value->application > UMI_UI_MOSAIC_APP_OS) return UMI_STATUS_INVALID_STATE;
    if (value->row_span == 0U || value->column_span == 0U) return UMI_STATUS_INVALID_STATE;
    end_row = (uint32_t)value->row + (uint32_t)value->row_span;
    end_column = (uint32_t)value->column + (uint32_t)value->column_span;
    if (end_row > UMI_UI_MOSAIC_MAX_CELLS || end_column > UMI_UI_MOSAIC_MAX_CELLS) return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}

uint32_t umi_ui_mosaic_application_panel_pack_area(const UmiUiMosaicApplicationPanelPack *value) {
    if (value == NULL) return 0U;
    return (uint32_t)value->row_span * (uint32_t)value->column_span;
}
