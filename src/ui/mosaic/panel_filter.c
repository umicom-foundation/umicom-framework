/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/mosaic/panel_filter.c
 *
 * PURPOSE:
 *   Define toolkit-neutral panel filter contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/mosaic/panel_filter.h"
#include <string.h>

void umi_ui_mosaic_panel_filter_init(UmiUiMosaicPanelFilter *value) {
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->application = UMI_UI_MOSAIC_APP_FRAMEWORK;
    value->enabled = true;
}

UmiStatus umi_ui_mosaic_panel_filter_set(UmiUiMosaicPanelFilter *value, const char *id, const char *title) {
    UmiStatus status;
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_mosaic_copy_text(value->id, sizeof(value->id), id);
    if (status != UMI_STATUS_OK) return status;
    return umi_ui_mosaic_copy_text(value->title, sizeof(value->title), title);
}

UmiStatus umi_ui_mosaic_panel_filter_validate(const UmiUiMosaicPanelFilter *value) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_ui_mosaic_id_is_valid(value->id) || value->title[0] == '\0') return UMI_STATUS_INVALID_STATE;
    if (value->application < UMI_UI_MOSAIC_APP_FRAMEWORK || value->application > UMI_UI_MOSAIC_APP_OS) return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

uint32_t umi_ui_mosaic_panel_filter_rank(const UmiUiMosaicPanelFilter *value, uint32_t context_boost) {
    if (value == NULL || !value->enabled) return 0U;
    return value->priority + context_boost + 1U;
}
