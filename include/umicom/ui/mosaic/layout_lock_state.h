/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/layout_lock_state.h
 *
 * PURPOSE:
 *   Define toolkit-neutral layout lock state contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_LAYOUT_LOCK_STATE_H
#define UMICOM_UI_MOSAIC_LAYOUT_LOCK_STATE_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUiMosaicLayoutLockState {
    char id[UMI_UI_MOSAIC_ID_CAPACITY];
    char name[UMI_UI_MOSAIC_TEXT_CAPACITY];
    uint32_t revision;
    uint32_t item_count;
    bool locked;
} UmiUiMosaicLayoutLockState;

/* Initializes versioned layout state used by layout lock state. */
void umi_ui_mosaic_layout_lock_state_init(UmiUiMosaicLayoutLockState *value);
/* Assigns the stable layout identity and display name. */
UmiStatus umi_ui_mosaic_layout_lock_state_set(UmiUiMosaicLayoutLockState *value, const char *id, const char *name);
/* Validates identity and bounded layout cardinality. */
UmiStatus umi_ui_mosaic_layout_lock_state_validate(const UmiUiMosaicLayoutLockState *value);
/* Advances the revision after a committed edit. */
UmiStatus umi_ui_mosaic_layout_lock_state_touch(UmiUiMosaicLayoutLockState *value);

#ifdef __cplusplus
}
#endif
#endif
