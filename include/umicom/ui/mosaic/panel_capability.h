/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/panel_capability.h
 *
 * PURPOSE:
 *   Define toolkit-neutral panel capability contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_PANEL_CAPABILITY_H
#define UMICOM_UI_MOSAIC_PANEL_CAPABILITY_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUiMosaicPanelCapability {
    char id[UMI_UI_MOSAIC_ID_CAPACITY];
    char title[UMI_UI_MOSAIC_TEXT_CAPACITY];
    UmiUiMosaicApplication application;
    uint32_t priority;
    bool enabled;
} UmiUiMosaicPanelCapability;

/* Initializes a bounded panel capability record with safe defaults. */
void umi_ui_mosaic_panel_capability_init(UmiUiMosaicPanelCapability *value);
/* Assigns the stable identifier and user-visible title. */
UmiStatus umi_ui_mosaic_panel_capability_set(UmiUiMosaicPanelCapability *value, const char *id, const char *title);
/* Verifies identifiers, application ownership and enabled state. */
UmiStatus umi_ui_mosaic_panel_capability_validate(const UmiUiMosaicPanelCapability *value);
/* Produces a deterministic ranking key used by catalogue/search surfaces. */
uint32_t umi_ui_mosaic_panel_capability_rank(const UmiUiMosaicPanelCapability *value, uint32_t context_boost);

#ifdef __cplusplus
}
#endif
#endif
