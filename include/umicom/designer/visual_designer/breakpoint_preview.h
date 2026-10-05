/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/breakpoint_preview.h
 *
 * PURPOSE:
 *   Resolve a named responsive preview breakpoint for the visual canvas.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_BREAKPOINT_PREVIEW_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_BREAKPOINT_PREVIEW_H
#include "umicom/designer/visual_designer/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer breakpoint preview data shared with callers of this public contract.
 */
typedef struct UmiRadBreakpointPreview {
    char breakpoint_id[UMI_RAD_ID_CAPACITY];
    UmiRadSize viewport;
    uint32_t dpi;
    bool touch;
} UmiRadBreakpointPreview;
/**
 * Initialise visual designer breakpoint preview from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_rad_breakpoint_preview_init(UmiRadBreakpointPreview *item);
/**
 * Check that visual designer breakpoint preview satisfies its contract before another service relies
 * on it.
 */
int umi_rad_breakpoint_preview_is_valid(const UmiRadBreakpointPreview *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_rad_breakpoint_preview_archive_encode(const UmiRadBreakpointPreview *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_rad_breakpoint_preview_archive_decode(const void *bytes, size_t byte_count,
    UmiRadBreakpointPreview *value);

#ifdef __cplusplus
}
#endif
#endif
