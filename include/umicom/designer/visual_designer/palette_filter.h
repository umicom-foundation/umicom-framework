/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/palette_filter.h
 *
 * PURPOSE:
 *   Filter the component palette by text, category and capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_PALETTE_FILTER_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_PALETTE_FILTER_H
#include "umicom/designer/visual_designer/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer palette filter data shared with callers of this public contract.
 */
typedef struct UmiRadPaletteFilter {
    char query[UMI_RAD_TEXT_CAPACITY];
    char category[UMI_RAD_ID_CAPACITY];
    char capability[UMI_RAD_ID_CAPACITY];
    bool favourites_only;
} UmiRadPaletteFilter;
/**
 * Initialise visual designer palette filter from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_palette_filter_init(UmiRadPaletteFilter *item);
/**
 * Check that visual designer palette filter satisfies its contract before another service relies on
 * it.
 */
int umi_rad_palette_filter_is_valid(const UmiRadPaletteFilter *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_rad_palette_filter_archive_encode(const UmiRadPaletteFilter *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_rad_palette_filter_archive_decode(const void *bytes, size_t byte_count,
    UmiRadPaletteFilter *value);

#ifdef __cplusplus
}
#endif
#endif
