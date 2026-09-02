/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/layout_catalogue.h
 *
 * PURPOSE:
 *   Define toolkit-neutral layout catalogue contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_LAYOUT_CATALOGUE_H
#define UMICOM_UI_MOSAIC_LAYOUT_CATALOGUE_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ui mosaic layout catalogue data shared with callers of this public
 * contract.
 */
typedef struct UmiUiMosaicLayoutCatalogue {
    char id[UMI_UI_MOSAIC_ID_CAPACITY];
    char name[UMI_UI_MOSAIC_TEXT_CAPACITY];
    uint32_t revision;
    uint32_t item_count;
    bool locked;
} UmiUiMosaicLayoutCatalogue;

/* Initializes versioned layout state used by layout catalogue. */
void umi_ui_mosaic_layout_catalogue_init(UmiUiMosaicLayoutCatalogue *value);
/* Assigns the stable layout identity and display name. */
UmiStatus umi_ui_mosaic_layout_catalogue_set(UmiUiMosaicLayoutCatalogue *value, const char *id, const char *name);
/* Validates identity and bounded layout cardinality. */
UmiStatus umi_ui_mosaic_layout_catalogue_validate(const UmiUiMosaicLayoutCatalogue *value);
/* Advances the revision after a committed edit. */
UmiStatus umi_ui_mosaic_layout_catalogue_touch(UmiUiMosaicLayoutCatalogue *value);

#ifdef __cplusplus
}
#endif
#endif
