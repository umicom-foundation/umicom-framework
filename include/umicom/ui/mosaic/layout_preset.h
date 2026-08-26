/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/layout_preset.h
 *
 * PURPOSE:
 *   Define toolkit-neutral layout preset contracts for the Framework-owned workbench mosaic platform.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_LAYOUT_PRESET_H
#define UMICOM_UI_MOSAIC_LAYOUT_PRESET_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUiMosaicLayoutPreset {
    char id[UMI_UI_MOSAIC_ID_CAPACITY];
    char name[UMI_UI_MOSAIC_TEXT_CAPACITY];
    uint32_t revision;
    uint32_t item_count;
    bool locked;
} UmiUiMosaicLayoutPreset;

/* Initializes versioned layout state used by layout preset. */
void umi_ui_mosaic_layout_preset_init(UmiUiMosaicLayoutPreset *value);
/* Assigns the stable layout identity and display name. */
UmiStatus umi_ui_mosaic_layout_preset_set(UmiUiMosaicLayoutPreset *value, const char *id, const char *name);
/* Validates identity and bounded layout cardinality. */
UmiStatus umi_ui_mosaic_layout_preset_validate(const UmiUiMosaicLayoutPreset *value);
/* Advances the revision after a committed edit. */
UmiStatus umi_ui_mosaic_layout_preset_touch(UmiUiMosaicLayoutPreset *value);

#ifdef __cplusplus
}
#endif
#endif
