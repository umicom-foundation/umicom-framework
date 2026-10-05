/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/layout_search.h
 *
 * PURPOSE:
 *   Define toolkit-neutral layout search contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_LAYOUT_SEARCH_H
#define UMICOM_UI_MOSAIC_LAYOUT_SEARCH_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ui mosaic layout search data shared with callers of this public contract.
 */
typedef struct UmiUiMosaicLayoutSearch {
    char id[UMI_UI_MOSAIC_ID_CAPACITY];
    char name[UMI_UI_MOSAIC_TEXT_CAPACITY];
    uint32_t revision;
    uint32_t item_count;
    bool locked;
} UmiUiMosaicLayoutSearch;

/* Initializes versioned layout state used by layout search. */
void umi_ui_mosaic_layout_search_init(UmiUiMosaicLayoutSearch *value);
/* Assigns the stable layout identity and display name. */
UmiStatus umi_ui_mosaic_layout_search_set(UmiUiMosaicLayoutSearch *value, const char *id, const char *name);
/* Validates identity and bounded layout cardinality. */
UmiStatus umi_ui_mosaic_layout_search_validate(const UmiUiMosaicLayoutSearch *value);
/* Advances the revision after a committed edit. */
UmiStatus umi_ui_mosaic_layout_search_touch(UmiUiMosaicLayoutSearch *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_mosaic_layout_search_archive_encode(const UmiUiMosaicLayoutSearch *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_mosaic_layout_search_archive_decode(const void *bytes, size_t byte_count,
    UmiUiMosaicLayoutSearch *value);

#ifdef __cplusplus
}
#endif
#endif
