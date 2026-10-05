/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/resize_handle.h
 *
 * PURPOSE:
 *   Describe resize-handle semantics without depending on a toolkit cursor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_RESIZE_HANDLE_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_RESIZE_HANDLE_H
#include "umicom/designer/visual_designer/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer resize handle data shared with callers of this public contract.
 */
typedef struct UmiRadResizeHandle {
    uint32_t edges;
    UmiRadPoint location;
    bool enabled;
} UmiRadResizeHandle;
/**
 * Initialise visual designer resize handle from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_resize_handle_init(UmiRadResizeHandle *item);
/**
 * Check that visual designer resize handle satisfies its contract before another service relies on it.
 */
int umi_rad_resize_handle_is_valid(const UmiRadResizeHandle *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_rad_resize_handle_archive_encode(const UmiRadResizeHandle *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_rad_resize_handle_archive_decode(const void *bytes, size_t byte_count,
    UmiRadResizeHandle *value);

#ifdef __cplusplus
}
#endif
#endif
