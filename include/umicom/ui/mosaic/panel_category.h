/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/panel_category.h
 *
 * PURPOSE:
 *   Define toolkit-neutral panel category contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_PANEL_CATEGORY_H
#define UMICOM_UI_MOSAIC_PANEL_CATEGORY_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ui mosaic panel category data shared with callers of this public contract.
 */
typedef struct UmiUiMosaicPanelCategory {
    char id[UMI_UI_MOSAIC_ID_CAPACITY];
    char title[UMI_UI_MOSAIC_TEXT_CAPACITY];
    UmiUiMosaicApplication application;
    uint32_t priority;
    bool enabled;
} UmiUiMosaicPanelCategory;

/* Initializes a bounded panel category record with safe defaults. */
void umi_ui_mosaic_panel_category_init(UmiUiMosaicPanelCategory *value);
/* Assigns the stable identifier and user-visible title. */
UmiStatus umi_ui_mosaic_panel_category_set(UmiUiMosaicPanelCategory *value, const char *id, const char *title);
/* Verifies identifiers, application ownership and enabled state. */
UmiStatus umi_ui_mosaic_panel_category_validate(const UmiUiMosaicPanelCategory *value);
/* Produces a deterministic ranking key used by catalogue/search surfaces. */
uint32_t umi_ui_mosaic_panel_category_rank(const UmiUiMosaicPanelCategory *value, uint32_t context_boost);

#ifdef __cplusplus
}
#endif
#endif
