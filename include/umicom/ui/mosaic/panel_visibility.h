/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/panel_visibility.h
 *
 * PURPOSE:
 *   Define toolkit-neutral panel visibility contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_PANEL_VISIBILITY_H
#define UMICOM_UI_MOSAIC_PANEL_VISIBILITY_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUiMosaicPanelVisibility {
    char id[UMI_UI_MOSAIC_ID_CAPACITY];
    char title[UMI_UI_MOSAIC_TEXT_CAPACITY];
    UmiUiMosaicApplication application;
    uint32_t priority;
    bool enabled;
} UmiUiMosaicPanelVisibility;

/* Initializes a bounded panel visibility record with safe defaults. */
void umi_ui_mosaic_panel_visibility_init(UmiUiMosaicPanelVisibility *value);
/* Assigns the stable identifier and user-visible title. */
UmiStatus umi_ui_mosaic_panel_visibility_set(UmiUiMosaicPanelVisibility *value, const char *id, const char *title);
/* Verifies identifiers, application ownership and enabled state. */
UmiStatus umi_ui_mosaic_panel_visibility_validate(const UmiUiMosaicPanelVisibility *value);
/* Produces a deterministic ranking key used by catalogue/search surfaces. */
uint32_t umi_ui_mosaic_panel_visibility_rank(const UmiUiMosaicPanelVisibility *value, uint32_t context_boost);

#ifdef __cplusplus
}
#endif
#endif
