/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/split_ratio.h
 *
 * PURPOSE:
 *   Define toolkit-neutral split ratio contracts for the Framework-owned workbench mosaic platform.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_SPLIT_RATIO_H
#define UMICOM_UI_MOSAIC_SPLIT_RATIO_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUiMosaicSplitRatio {
    char id[UMI_UI_MOSAIC_ID_CAPACITY];
    char name[UMI_UI_MOSAIC_TEXT_CAPACITY];
    uint32_t revision;
    uint32_t item_count;
    bool locked;
} UmiUiMosaicSplitRatio;

/* Initializes versioned layout state used by split ratio. */
void umi_ui_mosaic_split_ratio_init(UmiUiMosaicSplitRatio *value);
/* Assigns the stable layout identity and display name. */
UmiStatus umi_ui_mosaic_split_ratio_set(UmiUiMosaicSplitRatio *value, const char *id, const char *name);
/* Validates identity and bounded layout cardinality. */
UmiStatus umi_ui_mosaic_split_ratio_validate(const UmiUiMosaicSplitRatio *value);
/* Advances the revision after a committed edit. */
UmiStatus umi_ui_mosaic_split_ratio_touch(UmiUiMosaicSplitRatio *value);

#ifdef __cplusplus
}
#endif
#endif
