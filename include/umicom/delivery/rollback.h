/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/rollback.h
 *
 * PURPOSE:
 *   Represent and validate rollback requests between immutable installed generations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Rollback is an explicit operation with a reason and approval state rather than an ad-hoc file copy.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_ROLLBACK_H
#define INCLUDE_UMICOM_DELIVERY_ROLLBACK_H

#include "umicom/base/status.h"
#include "umicom/base/value_archive.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the rollback plan data shared with callers of this public contract.
 */
typedef struct UmiRollbackPlan {
    uint64_t current_generation;
    uint64_t target_generation;
    char reason[UMI_DELIVERY_TEXT_CAPACITY];
    int approved;
} UmiRollbackPlan;

/**
 * Initialise rollback plan from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_rollback_plan_init(UmiRollbackPlan *plan,
                                 uint64_t current_generation,
                                 uint64_t target_generation,
                                 const char *reason);
/**
 * Provide the rollback plan approve operation used by this module and its client
 * applications.
 */
UmiStatus umi_rollback_plan_approve(UmiRollbackPlan *plan);
/**
 * Check that rollback plan satisfies its contract before another service relies on it.
 */
int umi_rollback_plan_valid(const UmiRollbackPlan *plan);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_rollback_plan_archive_encode(const UmiRollbackPlan *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_rollback_plan_archive_decode(const void *bytes, size_t byte_count,
    UmiRollbackPlan *value);

#ifdef __cplusplus
}
#endif

#endif
