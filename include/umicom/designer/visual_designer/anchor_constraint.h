/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/anchor_constraint.h
 *
 * PURPOSE:
 *   Describe edge anchors for adaptive layouts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_ANCHOR_CONSTRAINT_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_ANCHOR_CONSTRAINT_H
#include "umicom/designer/visual_designer/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer anchor constraint data shared with callers of this public contract.
 */
typedef struct UmiRadAnchorConstraint {
    bool left;
    bool top;
    bool right;
    bool bottom;
    int32_t margin;
} UmiRadAnchorConstraint;
/**
 * Initialise visual designer anchor constraint from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_rad_anchor_constraint_init(UmiRadAnchorConstraint *item);
/**
 * Check that visual designer anchor constraint satisfies its contract before another service relies on
 * it.
 */
int umi_rad_anchor_constraint_is_valid(const UmiRadAnchorConstraint *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_rad_anchor_constraint_archive_encode(const UmiRadAnchorConstraint *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_rad_anchor_constraint_archive_decode(const void *bytes, size_t byte_count,
    UmiRadAnchorConstraint *value);

#ifdef __cplusplus
}
#endif
#endif
