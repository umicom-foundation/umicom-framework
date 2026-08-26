/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/panel_query.h
 *
 * PURPOSE:
 *   Define toolkit-neutral panel query contracts for the Framework-owned workbench mosaic platform.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_PANEL_QUERY_H
#define UMICOM_UI_MOSAIC_PANEL_QUERY_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUiMosaicPanelQuery {
    char id[UMI_UI_MOSAIC_ID_CAPACITY];
    char title[UMI_UI_MOSAIC_TEXT_CAPACITY];
    UmiUiMosaicApplication application;
    uint32_t priority;
    bool enabled;
} UmiUiMosaicPanelQuery;

/* Initializes a bounded panel query record with safe defaults. */
void umi_ui_mosaic_panel_query_init(UmiUiMosaicPanelQuery *value);
/* Assigns the stable identifier and user-visible title. */
UmiStatus umi_ui_mosaic_panel_query_set(UmiUiMosaicPanelQuery *value, const char *id, const char *title);
/* Verifies identifiers, application ownership and enabled state. */
UmiStatus umi_ui_mosaic_panel_query_validate(const UmiUiMosaicPanelQuery *value);
/* Produces a deterministic ranking key used by catalogue/search surfaces. */
uint32_t umi_ui_mosaic_panel_query_rank(const UmiUiMosaicPanelQuery *value, uint32_t context_boost);

#ifdef __cplusplus
}
#endif
#endif
