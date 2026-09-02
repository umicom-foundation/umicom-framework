/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/panel_pinning.h
 *
 * PURPOSE:
 *   Define toolkit-neutral panel pinning contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_PANEL_PINNING_H
#define UMICOM_UI_MOSAIC_PANEL_PINNING_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ui mosaic panel pinning data shared with callers of this public contract.
 */
typedef struct UmiUiMosaicPanelPinning {
    char id[UMI_UI_MOSAIC_ID_CAPACITY];
    char title[UMI_UI_MOSAIC_TEXT_CAPACITY];
    UmiUiMosaicApplication application;
    uint32_t priority;
    bool enabled;
} UmiUiMosaicPanelPinning;

/* Initializes a bounded panel pinning record with safe defaults. */
void umi_ui_mosaic_panel_pinning_init(UmiUiMosaicPanelPinning *value);
/* Assigns the stable identifier and user-visible title. */
UmiStatus umi_ui_mosaic_panel_pinning_set(UmiUiMosaicPanelPinning *value, const char *id, const char *title);
/* Verifies identifiers, application ownership and enabled state. */
UmiStatus umi_ui_mosaic_panel_pinning_validate(const UmiUiMosaicPanelPinning *value);
/* Produces a deterministic ranking key used by catalogue/search surfaces. */
uint32_t umi_ui_mosaic_panel_pinning_rank(const UmiUiMosaicPanelPinning *value, uint32_t context_boost);

#ifdef __cplusplus
}
#endif
#endif
