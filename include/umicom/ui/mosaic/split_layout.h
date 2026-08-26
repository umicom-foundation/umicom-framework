/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/split_layout.h
 *
 * PURPOSE:
 *   Define toolkit-neutral split layout contracts for the Framework-owned workbench mosaic platform.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_SPLIT_LAYOUT_H
#define UMICOM_UI_MOSAIC_SPLIT_LAYOUT_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUiMosaicSplitLayout {
    char id[UMI_UI_MOSAIC_ID_CAPACITY];
    char name[UMI_UI_MOSAIC_TEXT_CAPACITY];
    uint32_t revision;
    uint32_t item_count;
    bool locked;
} UmiUiMosaicSplitLayout;

/* Initializes versioned layout state used by split layout. */
void umi_ui_mosaic_split_layout_init(UmiUiMosaicSplitLayout *value);
/* Assigns the stable layout identity and display name. */
UmiStatus umi_ui_mosaic_split_layout_set(UmiUiMosaicSplitLayout *value, const char *id, const char *name);
/* Validates identity and bounded layout cardinality. */
UmiStatus umi_ui_mosaic_split_layout_validate(const UmiUiMosaicSplitLayout *value);
/* Advances the revision after a committed edit. */
UmiStatus umi_ui_mosaic_split_layout_touch(UmiUiMosaicSplitLayout *value);

#ifdef __cplusplus
}
#endif
#endif
