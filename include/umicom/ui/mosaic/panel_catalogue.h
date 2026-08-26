/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/panel_catalogue.h
 *
 * PURPOSE:
 *   Provide a searchable bounded catalogue of Framework panels contributed by any thin Umicom application.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_PANEL_CATALOGUE_H
#define UMICOM_UI_MOSAIC_PANEL_CATALOGUE_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUiMosaicPanelCatalogue {
    char id[UMI_UI_MOSAIC_ID_CAPACITY];
    char title[UMI_UI_MOSAIC_TEXT_CAPACITY];
    UmiUiMosaicApplication application;
    uint32_t priority;
    bool enabled;
} UmiUiMosaicPanelCatalogue;

/* Initializes a bounded panel catalogue record with safe defaults. */
void umi_ui_mosaic_panel_catalogue_init(UmiUiMosaicPanelCatalogue *value);
/* Assigns the stable identifier and user-visible title. */
UmiStatus umi_ui_mosaic_panel_catalogue_set(UmiUiMosaicPanelCatalogue *value, const char *id, const char *title);
/* Verifies identifiers, application ownership and enabled state. */
UmiStatus umi_ui_mosaic_panel_catalogue_validate(const UmiUiMosaicPanelCatalogue *value);
/* Produces a deterministic ranking key used by catalogue/search surfaces. */
uint32_t umi_ui_mosaic_panel_catalogue_rank(const UmiUiMosaicPanelCatalogue *value, uint32_t context_boost);

#ifdef __cplusplus
}
#endif
#endif
