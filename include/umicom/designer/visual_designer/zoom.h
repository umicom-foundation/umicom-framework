/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/zoom.h
 *
 * PURPOSE:
 *   Provide bounded zoom policy for visual authoring surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_ZOOM_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_ZOOM_H
#include "umicom/designer/visual_designer/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer zoom policy data shared with callers of this public contract.
 */
typedef struct UmiRadZoomPolicy {
    double minimum;
    double maximum;
    double current;
} UmiRadZoomPolicy;
/**
 * Initialise visual designer zoom from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_rad_zoom_init(UmiRadZoomPolicy *item);
/**
 * Check that visual designer zoom satisfies its contract before another service relies on it.
 */
int umi_rad_zoom_is_valid(const UmiRadZoomPolicy *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_rad_zoom_archive_encode(const UmiRadZoomPolicy *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_rad_zoom_archive_decode(const void *bytes, size_t byte_count,
    UmiRadZoomPolicy *value);

#ifdef __cplusplus
}
#endif
#endif
