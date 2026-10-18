/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/canvas.h
 *
 * PURPOSE:
 *   Describe a visual application design canvas and its revision state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_CANVAS_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_CANVAS_H
#include "umicom/designer/visual_designer/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer canvas data shared with callers of this public contract.
 */
typedef struct UmiRadCanvas {
    char document_id[UMI_RAD_ID_CAPACITY];
    char root_component_id[UMI_RAD_ID_CAPACITY];
    uint64_t revision;
    bool dirty;
} UmiRadCanvas;
/**
 * Initialise visual designer canvas from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_rad_canvas_init(UmiRadCanvas *item);
/**
 * Check that visual designer canvas satisfies its contract before another service relies on it.
 */
int umi_rad_canvas_is_valid(const UmiRadCanvas *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_rad_canvas_archive_encode(const UmiRadCanvas *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_rad_canvas_archive_decode(const void *bytes, size_t byte_count,
    UmiRadCanvas *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiRadCanvas object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_rad_canvas_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiRadCanvas *workspace, UmiRadCanvas *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_rad_canvas_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiRadCanvas *expected, UmiRadCanvas *value);


#ifdef __cplusplus
}
#endif
#endif
