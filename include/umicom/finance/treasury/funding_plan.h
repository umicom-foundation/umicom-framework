/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/funding_plan.h
 *
 * PURPOSE:
 *   Represent a funded amount and enforce that allocations do not exceed requirement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_FUNDING_PLAN_H
#define UMICOM_FINANCE_TREASURY_FUNDING_PLAN_H
#include "umicom/finance/treasury/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury funding plan data shared with callers of this public contract.
 */
typedef struct UmiTreasuryFundingPlan {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t requirement_minor;
    int64_t allocated_minor;
} UmiTreasuryFundingPlan;
/**
 * Initialise treasury funding plan from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_treasury_funding_plan_init(UmiTreasuryFundingPlan *value,
    const char *id,
    int64_t requirement_minor,
    int64_t allocated_minor);
/**
 * Check that treasury funding plan satisfies its contract before another service relies on
 * it.
 */
bool umi_treasury_funding_plan_valid(const UmiTreasuryFundingPlan *value);
/**
 * Provide the treasury funding plan remaining minor operation used by this module and its
 * client applications.
 */
int64_t umi_treasury_funding_plan_remaining_minor(const UmiTreasuryFundingPlan *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_treasury_funding_plan_archive_encode(const UmiTreasuryFundingPlan *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_treasury_funding_plan_archive_decode(const void *bytes, size_t byte_count,
    UmiTreasuryFundingPlan *value);

#ifdef __cplusplus
}
#endif
#endif
