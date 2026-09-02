/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/settlement_failure.c
 *
 * PURPOSE:
 *   Implement record failed settlement exposure, age and retry eligibility.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/settlement_failure.h"
#include <string.h>
/*
 * Initialise treasury settlement failure from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_settlement_failure_init(UmiTreasurySettlementFailure *value,
    const char *id,
    int64_t exposure_minor,
    uint32_t age_days,
    uint32_t retry_count) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->exposure_minor=exposure_minor;
    value->age_days=age_days;
    value->retry_count=retry_count;
    return umi_treasury_settlement_failure_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury settlement failure satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_settlement_failure_valid(const UmiTreasurySettlementFailure *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->exposure_minor >= 0);
}

/*
 * Provide the treasury settlement failure aged operation used by this module and its
 * client applications.
 */
bool umi_treasury_settlement_failure_aged(const UmiTreasurySettlementFailure *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (bool)0;
    return value->age_days > 2U;
}
