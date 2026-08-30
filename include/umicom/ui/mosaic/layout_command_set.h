/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/layout_command_set.h
 *
 * PURPOSE:
 *   Define toolkit-neutral layout command set contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_LAYOUT_COMMAND_SET_H
#define UMICOM_UI_MOSAIC_LAYOUT_COMMAND_SET_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUiMosaicLayoutCommandSet {
    char id[UMI_UI_MOSAIC_ID_CAPACITY];
    char label[UMI_UI_MOSAIC_TEXT_CAPACITY];
    uint32_t ordinal;
    bool enabled;
    bool requires_edit_mode;
} UmiUiMosaicLayoutCommandSet;

/* Initializes a stable Layout Designer command descriptor. */
void umi_ui_mosaic_layout_command_set_init(UmiUiMosaicLayoutCommandSet *value);
/* Assigns command identity and display label. */
UmiStatus umi_ui_mosaic_layout_command_set_set(UmiUiMosaicLayoutCommandSet *value, const char *id, const char *label);
/* Validates command identity before exposing it to menus/toolbars. */
UmiStatus umi_ui_mosaic_layout_command_set_validate(const UmiUiMosaicLayoutCommandSet *value);
/* Evaluates whether the command can run in the current edit mode. */
int umi_ui_mosaic_layout_command_set_can_execute(const UmiUiMosaicLayoutCommandSet *value, UmiUiMosaicEditMode mode);

#ifdef __cplusplus
}
#endif
#endif
