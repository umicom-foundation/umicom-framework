/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/window_placement.h
 *
 * PURPOSE:
 *   Define toolkit-neutral window placement contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_WINDOW_PLACEMENT_H
#define UMICOM_UI_MOSAIC_WINDOW_PLACEMENT_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUiMosaicWindowPlacement {
    char id[UMI_UI_MOSAIC_ID_CAPACITY];
    char name[UMI_UI_MOSAIC_TEXT_CAPACITY];
    uint32_t revision;
    uint32_t item_count;
    bool locked;
} UmiUiMosaicWindowPlacement;

/* Initializes versioned layout state used by window placement. */
void umi_ui_mosaic_window_placement_init(UmiUiMosaicWindowPlacement *value);
/* Assigns the stable layout identity and display name. */
UmiStatus umi_ui_mosaic_window_placement_set(UmiUiMosaicWindowPlacement *value, const char *id, const char *name);
/* Validates identity and bounded layout cardinality. */
UmiStatus umi_ui_mosaic_window_placement_validate(const UmiUiMosaicWindowPlacement *value);
/* Advances the revision after a committed edit. */
UmiStatus umi_ui_mosaic_window_placement_touch(UmiUiMosaicWindowPlacement *value);

#ifdef __cplusplus
}
#endif
#endif
