/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/drop_target.h
 *
 * PURPOSE:
 *   Represent validated parent/slot destinations during component drag and drop.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_DROP_TARGET_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_DROP_TARGET_H
#include "umicom/designer/visual_designer/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer drop target data shared with callers of this public contract.
 */
typedef struct UmiRadDropTarget {
    char parent_id[UMI_RAD_ID_CAPACITY];
    char slot_id[UMI_RAD_ID_CAPACITY];
    UmiRadRect bounds;
    bool accepted;
} UmiRadDropTarget;
/**
 * Initialise visual designer drop target from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_drop_target_init(UmiRadDropTarget *item);
/**
 * Check that visual designer drop target satisfies its contract before another service relies on it.
 */
int umi_rad_drop_target_is_valid(const UmiRadDropTarget *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_rad_drop_target_archive_encode(const UmiRadDropTarget *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_rad_drop_target_archive_decode(const void *bytes, size_t byte_count,
    UmiRadDropTarget *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiRadDropTarget object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_rad_drop_target_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiRadDropTarget *workspace, UmiRadDropTarget *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_rad_drop_target_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiRadDropTarget *expected, UmiRadDropTarget *value);


#ifdef __cplusplus
}
#endif
#endif
