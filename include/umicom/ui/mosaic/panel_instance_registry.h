/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/panel_instance_registry.h
 *
 * PURPOSE:
 *   Define toolkit-neutral panel instance registry contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_PANEL_INSTANCE_REGISTRY_H
#define UMICOM_UI_MOSAIC_PANEL_INSTANCE_REGISTRY_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUiMosaicPanelInstanceRegistry {
    char id[UMI_UI_MOSAIC_ID_CAPACITY];
    char title[UMI_UI_MOSAIC_TEXT_CAPACITY];
    UmiUiMosaicApplication application;
    uint32_t priority;
    bool enabled;
} UmiUiMosaicPanelInstanceRegistry;

/* Initializes a bounded panel instance registry record with safe defaults. */
void umi_ui_mosaic_panel_instance_registry_init(UmiUiMosaicPanelInstanceRegistry *value);
/* Assigns the stable identifier and user-visible title. */
UmiStatus umi_ui_mosaic_panel_instance_registry_set(UmiUiMosaicPanelInstanceRegistry *value, const char *id, const char *title);
/* Verifies identifiers, application ownership and enabled state. */
UmiStatus umi_ui_mosaic_panel_instance_registry_validate(const UmiUiMosaicPanelInstanceRegistry *value);
/* Produces a deterministic ranking key used by catalogue/search surfaces. */
uint32_t umi_ui_mosaic_panel_instance_registry_rank(const UmiUiMosaicPanelInstanceRegistry *value, uint32_t context_boost);

#ifdef __cplusplus
}
#endif
#endif
