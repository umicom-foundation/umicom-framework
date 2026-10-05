/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/grid.h
 *
 * PURPOSE:
 *   Describe configurable design-time grid spacing and visibility.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_GRID_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_GRID_H
#include "umicom/designer/visual_designer/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual design grid data shared with callers of this public contract.
 */
typedef struct UmiRadDesignGrid {
    int32_t spacing_x;
    int32_t spacing_y;
    bool visible;
    bool snap_enabled;
} UmiRadDesignGrid;
/**
 * Initialise visual designer grid from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_rad_grid_init(UmiRadDesignGrid *item);
/**
 * Check that visual designer grid satisfies its contract before another service relies on it.
 */
int umi_rad_grid_is_valid(const UmiRadDesignGrid *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_rad_grid_archive_encode(const UmiRadDesignGrid *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_rad_grid_archive_decode(const void *bytes, size_t byte_count,
    UmiRadDesignGrid *value);

#ifdef __cplusplus
}
#endif
#endif
