/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/floating_panel.h
 *
 * PURPOSE:
 *   Define toolkit-neutral floating panel contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_FLOATING_PANEL_H
#define UMICOM_UI_MOSAIC_FLOATING_PANEL_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUiMosaicFloatingPanel {
    char id[UMI_UI_MOSAIC_ID_CAPACITY];
    char name[UMI_UI_MOSAIC_TEXT_CAPACITY];
    uint32_t revision;
    uint32_t item_count;
    bool locked;
} UmiUiMosaicFloatingPanel;

/* Initializes versioned layout state used by floating panel. */
void umi_ui_mosaic_floating_panel_init(UmiUiMosaicFloatingPanel *value);
/* Assigns the stable layout identity and display name. */
UmiStatus umi_ui_mosaic_floating_panel_set(UmiUiMosaicFloatingPanel *value, const char *id, const char *name);
/* Validates identity and bounded layout cardinality. */
UmiStatus umi_ui_mosaic_floating_panel_validate(const UmiUiMosaicFloatingPanel *value);
/* Advances the revision after a committed edit. */
UmiStatus umi_ui_mosaic_floating_panel_touch(UmiUiMosaicFloatingPanel *value);

#ifdef __cplusplus
}
#endif
#endif
