/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/panel_sort.h
 *
 * PURPOSE:
 *   Define toolkit-neutral panel sort contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_PANEL_SORT_H
#define UMICOM_UI_MOSAIC_PANEL_SORT_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUiMosaicPanelSort {
    char id[UMI_UI_MOSAIC_ID_CAPACITY];
    char title[UMI_UI_MOSAIC_TEXT_CAPACITY];
    UmiUiMosaicApplication application;
    uint32_t priority;
    bool enabled;
} UmiUiMosaicPanelSort;

/* Initializes a bounded panel sort record with safe defaults. */
void umi_ui_mosaic_panel_sort_init(UmiUiMosaicPanelSort *value);
/* Assigns the stable identifier and user-visible title. */
UmiStatus umi_ui_mosaic_panel_sort_set(UmiUiMosaicPanelSort *value, const char *id, const char *title);
/* Verifies identifiers, application ownership and enabled state. */
UmiStatus umi_ui_mosaic_panel_sort_validate(const UmiUiMosaicPanelSort *value);
/* Produces a deterministic ranking key used by catalogue/search surfaces. */
uint32_t umi_ui_mosaic_panel_sort_rank(const UmiUiMosaicPanelSort *value, uint32_t context_boost);

#ifdef __cplusplus
}
#endif
#endif
