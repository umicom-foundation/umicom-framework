/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/payments/cut_off_policy.c
 *
 * PURPOSE:
 *   Implement represent rail cut-off time and deferred-execution policy using minutes from midnight.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/cut_off_policy.h"
#include <string.h>
/*
 * Initialise payments cut off policy from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_payments_cut_off_policy_init(UmiPaymentsCutOffPolicy *value,
    const char *id,
    const char *rail_id,
    uint16_t cutoff_minute,
    bool defer_after_cutoff) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_payments_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_payments_id_assign(&value->rail_id,rail_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    value->cutoff_minute=cutoff_minute;
    value->defer_after_cutoff=defer_after_cutoff;
    return umi_payments_cut_off_policy_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that payments cut off policy satisfies its contract before another service relies
 * on it.
 */
bool umi_payments_cut_off_policy_valid(const UmiPaymentsCutOffPolicy *value) {
    return value!=NULL && (value->cutoff_minute<1440U);
}

/*
 * Provide the payments cut off policy accepts minute 600 operation used by this module and
 * its client applications.
 */
bool umi_payments_cut_off_policy_accepts_minute_600(const UmiPaymentsCutOffPolicy *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return 600U<=value->cutoff_minute;
}
