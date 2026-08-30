/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/detached_window.h
 *
 * PURPOSE:
 *   Define toolkit-neutral detached window contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_DETACHED_WINDOW_H
#define UMICOM_UI_MOSAIC_DETACHED_WINDOW_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUiMosaicDetachedWindow {
    char id[UMI_UI_MOSAIC_ID_CAPACITY];
    char name[UMI_UI_MOSAIC_TEXT_CAPACITY];
    uint32_t revision;
    uint32_t item_count;
    bool locked;
} UmiUiMosaicDetachedWindow;

/* Initializes versioned layout state used by detached window. */
void umi_ui_mosaic_detached_window_init(UmiUiMosaicDetachedWindow *value);
/* Assigns the stable layout identity and display name. */
UmiStatus umi_ui_mosaic_detached_window_set(UmiUiMosaicDetachedWindow *value, const char *id, const char *name);
/* Validates identity and bounded layout cardinality. */
UmiStatus umi_ui_mosaic_detached_window_validate(const UmiUiMosaicDetachedWindow *value);
/* Advances the revision after a committed edit. */
UmiStatus umi_ui_mosaic_detached_window_touch(UmiUiMosaicDetachedWindow *value);

#ifdef __cplusplus
}
#endif
#endif
