/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/panel_activation.h
 *
 * PURPOSE:
 *   Define toolkit-neutral panel activation contracts for the Framework-owned workbench mosaic platform.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_PANEL_ACTIVATION_H
#define UMICOM_UI_MOSAIC_PANEL_ACTIVATION_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUiMosaicPanelActivation {
    char id[UMI_UI_MOSAIC_ID_CAPACITY];
    char title[UMI_UI_MOSAIC_TEXT_CAPACITY];
    UmiUiMosaicApplication application;
    uint32_t priority;
    bool enabled;
} UmiUiMosaicPanelActivation;

/* Initializes a bounded panel activation record with safe defaults. */
void umi_ui_mosaic_panel_activation_init(UmiUiMosaicPanelActivation *value);
/* Assigns the stable identifier and user-visible title. */
UmiStatus umi_ui_mosaic_panel_activation_set(UmiUiMosaicPanelActivation *value, const char *id, const char *title);
/* Verifies identifiers, application ownership and enabled state. */
UmiStatus umi_ui_mosaic_panel_activation_validate(const UmiUiMosaicPanelActivation *value);
/* Produces a deterministic ranking key used by catalogue/search surfaces. */
uint32_t umi_ui_mosaic_panel_activation_rank(const UmiUiMosaicPanelActivation *value, uint32_t context_boost);

#ifdef __cplusplus
}
#endif
#endif
