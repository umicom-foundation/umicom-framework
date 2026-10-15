/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/generation_plan.h
 *
 * PURPOSE:
 *   Describe generated declarative/source artifacts before filesystem writes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_GENERATION_PLAN_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_GENERATION_PLAN_H
#include "umicom/designer/visual_designer/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer generation plan data shared with callers of this public contract.
 */
typedef struct UmiRadGenerationPlan {
    char application_id[UMI_RAD_ID_CAPACITY];
    char output_root[UMI_RAD_PATH_CAPACITY];
    size_t file_count;
    bool declarative_enabled;
    bool source_enabled;
} UmiRadGenerationPlan;
/**
 * Initialise visual designer generation plan from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_generation_plan_init(UmiRadGenerationPlan *item);
/**
 * Check that visual designer generation plan satisfies its contract before another service relies on
 * it.
 */
int umi_rad_generation_plan_is_valid(const UmiRadGenerationPlan *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_rad_generation_plan_archive_encode(const UmiRadGenerationPlan *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_rad_generation_plan_archive_decode(const void *bytes, size_t byte_count,
    UmiRadGenerationPlan *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_rad_generation_plan_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiRadGenerationPlan *expected, UmiRadGenerationPlan *value);


#ifdef __cplusplus
}
#endif
#endif
