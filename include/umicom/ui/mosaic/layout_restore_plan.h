/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/layout_restore_plan.h
 *
 * PURPOSE:
 *   Plan safe layout restoration when monitors or panels from a saved workspace are unavailable.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_LAYOUT_RESTORE_PLAN_H
#define UMICOM_UI_MOSAIC_LAYOUT_RESTORE_PLAN_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUiMosaicLayoutRestorePlan {
    char id[UMI_UI_MOSAIC_ID_CAPACITY];
    char name[UMI_UI_MOSAIC_TEXT_CAPACITY];
    uint32_t revision;
    uint32_t item_count;
    bool locked;
} UmiUiMosaicLayoutRestorePlan;

/* Initializes versioned layout state used by layout restore plan. */
void umi_ui_mosaic_layout_restore_plan_init(UmiUiMosaicLayoutRestorePlan *value);
/* Assigns the stable layout identity and display name. */
UmiStatus umi_ui_mosaic_layout_restore_plan_set(UmiUiMosaicLayoutRestorePlan *value, const char *id, const char *name);
/* Validates identity and bounded layout cardinality. */
UmiStatus umi_ui_mosaic_layout_restore_plan_validate(const UmiUiMosaicLayoutRestorePlan *value);
/* Advances the revision after a committed edit. */
UmiStatus umi_ui_mosaic_layout_restore_plan_touch(UmiUiMosaicLayoutRestorePlan *value);

#ifdef __cplusplus
}
#endif
#endif
