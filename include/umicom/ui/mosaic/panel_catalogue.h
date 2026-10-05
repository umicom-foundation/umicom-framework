/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/panel_catalogue.h
 *
 * PURPOSE:
 *   Provide a searchable bounded catalogue of Framework panels contributed by any thin Umicom application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_PANEL_CATALOGUE_H
#define UMICOM_UI_MOSAIC_PANEL_CATALOGUE_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ui mosaic panel catalogue data shared with callers of this public
 * contract.
 */
typedef struct UmiUiMosaicPanelCatalogue {
    char id[UMI_UI_MOSAIC_ID_CAPACITY];
    char title[UMI_UI_MOSAIC_TEXT_CAPACITY];
    UmiUiMosaicApplication application;
    uint32_t priority;
    bool enabled;
} UmiUiMosaicPanelCatalogue;

/* Initializes a bounded panel catalogue record with safe defaults. */
void umi_ui_mosaic_panel_catalogue_init(UmiUiMosaicPanelCatalogue *value);
/* Assigns the stable identifier and user-visible title. */
UmiStatus umi_ui_mosaic_panel_catalogue_set(UmiUiMosaicPanelCatalogue *value, const char *id, const char *title);
/* Verifies identifiers, application ownership and enabled state. */
UmiStatus umi_ui_mosaic_panel_catalogue_validate(const UmiUiMosaicPanelCatalogue *value);
/* Produces a deterministic ranking key used by catalogue/search surfaces. */
uint32_t umi_ui_mosaic_panel_catalogue_rank(const UmiUiMosaicPanelCatalogue *value, uint32_t context_boost);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_mosaic_panel_catalogue_archive_encode(const UmiUiMosaicPanelCatalogue *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_mosaic_panel_catalogue_archive_decode(const void *bytes, size_t byte_count,
    UmiUiMosaicPanelCatalogue *value);

#ifdef __cplusplus
}
#endif
#endif
