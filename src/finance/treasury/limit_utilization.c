/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/limit_utilization.c
 *
 * PURPOSE:
 *   Implement calculate risk-limit utilisation using basis points.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/limit_utilization.h"
#include <string.h>
/*
 * Initialise treasury limit utilization from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_limit_utilization_init(UmiTreasuryLimitUtilization *value,
    const char *id,
    int64_t used_minor,
    int64_t limit_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->used_minor=used_minor;
    value->limit_minor=limit_minor;
    return umi_treasury_limit_utilization_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury limit utilization satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_limit_utilization_valid(const UmiTreasuryLimitUtilization *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->used_minor >= 0 && value->limit_minor > 0);
}

/*
 * Provide the treasury limit utilization utilization bps operation used by this module and
 * its client applications.
 */
uint32_t umi_treasury_limit_utilization_utilization_bps(const UmiTreasuryLimitUtilization *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (uint32_t)0;
    return (uint32_t)((value->used_minor * 10000) / value->limit_minor);
}
