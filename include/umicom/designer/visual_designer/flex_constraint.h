/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/flex_constraint.h
 *
 * PURPOSE:
 *   Describe renderer-neutral flexible-box growth and basis constraints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_FLEX_CONSTRAINT_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_FLEX_CONSTRAINT_H
#include "umicom/designer/visual_designer/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer flex constraint data shared with callers of this public contract.
 */
typedef struct UmiRadFlexConstraint {
    double grow;
    double shrink;
    int32_t basis;
    int32_t order;
} UmiRadFlexConstraint;
/**
 * Initialise visual designer flex constraint from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_flex_constraint_init(UmiRadFlexConstraint *item);
/**
 * Check that visual designer flex constraint satisfies its contract before another service relies on
 * it.
 */
int umi_rad_flex_constraint_is_valid(const UmiRadFlexConstraint *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_rad_flex_constraint_archive_encode(const UmiRadFlexConstraint *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_rad_flex_constraint_archive_decode(const void *bytes, size_t byte_count,
    UmiRadFlexConstraint *value);

#ifdef __cplusplus
}
#endif
#endif
