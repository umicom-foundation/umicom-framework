/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/perspective_catalogue.h
 *
 * PURPOSE:
 *   Define toolkit-neutral perspective catalogue contracts for the Framework-owned workbench mosaic platform.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_PERSPECTIVE_CATALOGUE_H
#define UMICOM_UI_MOSAIC_PERSPECTIVE_CATALOGUE_H

#include <stdbool.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUiMosaicPerspectiveCatalogue {
    char id[UMI_UI_MOSAIC_ID_CAPACITY];
    char name[UMI_UI_MOSAIC_TEXT_CAPACITY];
    char layout_id[UMI_UI_MOSAIC_ID_CAPACITY];
    UmiUiMosaicApplication application;
    bool available;
} UmiUiMosaicPerspectiveCatalogue;

/* Initializes a task/workspace descriptor without toolkit state. */
void umi_ui_mosaic_perspective_catalogue_init(UmiUiMosaicPerspectiveCatalogue *value);
/* Configures identity, default layout and application ownership. */
UmiStatus umi_ui_mosaic_perspective_catalogue_set(UmiUiMosaicPerspectiveCatalogue *value, const char *id, const char *name, const char *layout_id, UmiUiMosaicApplication application);
/* Validates that a selectable perspective/workspace has a valid layout. */
UmiStatus umi_ui_mosaic_perspective_catalogue_validate(const UmiUiMosaicPerspectiveCatalogue *value);

#ifdef __cplusplus
}
#endif
#endif
