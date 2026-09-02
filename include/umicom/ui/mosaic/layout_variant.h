/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/layout_variant.h
 *
 * PURPOSE:
 *   Define toolkit-neutral layout variant contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_LAYOUT_VARIANT_H
#define UMICOM_UI_MOSAIC_LAYOUT_VARIANT_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ui mosaic layout variant data shared with callers of this public contract.
 */
typedef struct UmiUiMosaicLayoutVariant {
    char id[UMI_UI_MOSAIC_ID_CAPACITY];
    char name[UMI_UI_MOSAIC_TEXT_CAPACITY];
    uint32_t revision;
    uint32_t item_count;
    bool locked;
} UmiUiMosaicLayoutVariant;

/* Initializes versioned layout state used by layout variant. */
void umi_ui_mosaic_layout_variant_init(UmiUiMosaicLayoutVariant *value);
/* Assigns the stable layout identity and display name. */
UmiStatus umi_ui_mosaic_layout_variant_set(UmiUiMosaicLayoutVariant *value, const char *id, const char *name);
/* Validates identity and bounded layout cardinality. */
UmiStatus umi_ui_mosaic_layout_variant_validate(const UmiUiMosaicLayoutVariant *value);
/* Advances the revision after a committed edit. */
UmiStatus umi_ui_mosaic_layout_variant_touch(UmiUiMosaicLayoutVariant *value);

#ifdef __cplusplus
}
#endif
#endif
