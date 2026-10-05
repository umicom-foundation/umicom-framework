/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/drag_session.h
 *
 * PURPOSE:
 *   Track a visual component drag operation from press through commit/cancel.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_DRAG_SESSION_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_DRAG_SESSION_H
#include "umicom/designer/visual_designer/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer drag session data shared with callers of this public contract.
 */
typedef struct UmiRadDragSession {
    char component_id[UMI_RAD_ID_CAPACITY];
    UmiRadPoint start;
    UmiRadPoint current;
    bool active;
} UmiRadDragSession;
/**
 * Initialise visual designer drag session from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_drag_session_init(UmiRadDragSession *item);
/**
 * Check that visual designer drag session satisfies its contract before another service relies on it.
 */
int umi_rad_drag_session_is_valid(const UmiRadDragSession *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_rad_drag_session_archive_encode(const UmiRadDragSession *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_rad_drag_session_archive_decode(const void *bytes, size_t byte_count,
    UmiRadDragSession *value);

#ifdef __cplusplus
}
#endif
#endif
