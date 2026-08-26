/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/mosaic/perspective_catalogue.c
 *
 * PURPOSE:
 *   Define toolkit-neutral perspective catalogue contracts for the Framework-owned workbench mosaic platform.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/mosaic/perspective_catalogue.h"
#include <string.h>

void umi_ui_mosaic_perspective_catalogue_init(UmiUiMosaicPerspectiveCatalogue *value) {
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->application = UMI_UI_MOSAIC_APP_FRAMEWORK;
    value->available = true;
}

UmiStatus umi_ui_mosaic_perspective_catalogue_set(UmiUiMosaicPerspectiveCatalogue *value, const char *id, const char *name, const char *layout_id, UmiUiMosaicApplication application) {
    UmiStatus status;
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_mosaic_copy_text(value->id, sizeof(value->id), id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_mosaic_copy_text(value->name, sizeof(value->name), name);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_mosaic_copy_text(value->layout_id, sizeof(value->layout_id), layout_id);
    if (status != UMI_STATUS_OK) return status;
    value->application = application;
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_mosaic_perspective_catalogue_validate(const UmiUiMosaicPerspectiveCatalogue *value) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!value->available || !umi_ui_mosaic_id_is_valid(value->id) || !umi_ui_mosaic_id_is_valid(value->layout_id) || value->name[0] == '\0') return UMI_STATUS_INVALID_STATE;
    if (value->application < UMI_UI_MOSAIC_APP_FRAMEWORK || value->application > UMI_UI_MOSAIC_APP_OS) return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
