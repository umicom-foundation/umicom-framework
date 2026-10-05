/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/cash_sweep_plan.h
 *
 * PURPOSE:
 *   Represent an executable cash sweep amount subject to a maximum.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_CASH_SWEEP_PLAN_H
#define UMICOM_FINANCE_TREASURY_CASH_SWEEP_PLAN_H
#include "umicom/finance/treasury/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury cash sweep plan data shared with callers of this public contract.
 */
typedef struct UmiTreasuryCashSweepPlan {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t requested_minor;
    int64_t maximum_minor;
} UmiTreasuryCashSweepPlan;
/**
 * Initialise treasury cash sweep plan from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_cash_sweep_plan_init(UmiTreasuryCashSweepPlan *value,
    const char *id,
    int64_t requested_minor,
    int64_t maximum_minor);
/**
 * Check that treasury cash sweep plan satisfies its contract before another service relies
 * on it.
 */
bool umi_treasury_cash_sweep_plan_valid(const UmiTreasuryCashSweepPlan *value);
/**
 * Provide the treasury cash sweep plan executable minor operation used by this module and
 * its client applications.
 */
int64_t umi_treasury_cash_sweep_plan_executable_minor(const UmiTreasuryCashSweepPlan *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_treasury_cash_sweep_plan_archive_encode(const UmiTreasuryCashSweepPlan *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_treasury_cash_sweep_plan_archive_decode(const void *bytes, size_t byte_count,
    UmiTreasuryCashSweepPlan *value);

#ifdef __cplusplus
}
#endif
#endif
