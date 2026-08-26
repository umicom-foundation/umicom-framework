/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/layout_edit_session.h
 *
 * PURPOSE:
 *   Define toolkit-neutral layout edit session contracts for the Framework-owned workbench mosaic platform.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_LAYOUT_EDIT_SESSION_H
#define UMICOM_UI_MOSAIC_LAYOUT_EDIT_SESSION_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUiMosaicLayoutEditSession {
    char id[UMI_UI_MOSAIC_ID_CAPACITY];
    char name[UMI_UI_MOSAIC_TEXT_CAPACITY];
    uint32_t revision;
    uint32_t item_count;
    bool locked;
} UmiUiMosaicLayoutEditSession;

/* Initializes versioned layout state used by layout edit session. */
void umi_ui_mosaic_layout_edit_session_init(UmiUiMosaicLayoutEditSession *value);
/* Assigns the stable layout identity and display name. */
UmiStatus umi_ui_mosaic_layout_edit_session_set(UmiUiMosaicLayoutEditSession *value, const char *id, const char *name);
/* Validates identity and bounded layout cardinality. */
UmiStatus umi_ui_mosaic_layout_edit_session_validate(const UmiUiMosaicLayoutEditSession *value);
/* Advances the revision after a committed edit. */
UmiStatus umi_ui_mosaic_layout_edit_session_touch(UmiUiMosaicLayoutEditSession *value);

#ifdef __cplusplus
}
#endif
#endif
