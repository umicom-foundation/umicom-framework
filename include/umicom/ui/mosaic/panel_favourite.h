/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/panel_favourite.h
 *
 * PURPOSE:
 *   Define toolkit-neutral panel favourite contracts for the Framework-owned workbench mosaic platform.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_PANEL_FAVOURITE_H
#define UMICOM_UI_MOSAIC_PANEL_FAVOURITE_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUiMosaicPanelFavourite {
    char id[UMI_UI_MOSAIC_ID_CAPACITY];
    char title[UMI_UI_MOSAIC_TEXT_CAPACITY];
    UmiUiMosaicApplication application;
    uint32_t priority;
    bool enabled;
} UmiUiMosaicPanelFavourite;

/* Initializes a bounded panel favourite record with safe defaults. */
void umi_ui_mosaic_panel_favourite_init(UmiUiMosaicPanelFavourite *value);
/* Assigns the stable identifier and user-visible title. */
UmiStatus umi_ui_mosaic_panel_favourite_set(UmiUiMosaicPanelFavourite *value, const char *id, const char *title);
/* Verifies identifiers, application ownership and enabled state. */
UmiStatus umi_ui_mosaic_panel_favourite_validate(const UmiUiMosaicPanelFavourite *value);
/* Produces a deterministic ranking key used by catalogue/search surfaces. */
uint32_t umi_ui_mosaic_panel_favourite_rank(const UmiUiMosaicPanelFavourite *value, uint32_t context_boost);

#ifdef __cplusplus
}
#endif
#endif
