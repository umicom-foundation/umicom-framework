/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/grid_constraint.h
 *
 * PURPOSE:
 *   Describe renderer-neutral grid row/column placement and spans.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_GRID_CONSTRAINT_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_GRID_CONSTRAINT_H
#include "umicom/designer/visual_designer/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer grid constraint data shared with callers of this public contract.
 */
typedef struct UmiRadGridConstraint {
    int32_t row;
    int32_t column;
    int32_t row_span;
    int32_t column_span;
} UmiRadGridConstraint;
/**
 * Initialise visual designer grid constraint from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_grid_constraint_init(UmiRadGridConstraint *item);
/**
 * Check that visual designer grid constraint satisfies its contract before another service relies on
 * it.
 */
int umi_rad_grid_constraint_is_valid(const UmiRadGridConstraint *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_rad_grid_constraint_archive_encode(const UmiRadGridConstraint *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_rad_grid_constraint_archive_decode(const void *bytes, size_t byte_count,
    UmiRadGridConstraint *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_rad_grid_constraint_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiRadGridConstraint *expected, UmiRadGridConstraint *value);


#ifdef __cplusplus
}
#endif
#endif
