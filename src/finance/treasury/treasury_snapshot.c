/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/treasury_snapshot.c
 *
 * PURPOSE:
 *   Implement capture aggregate cash, liquidity, risk and collateral state at a point in time.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/treasury_snapshot.h"
#include <string.h>
/*
 * Initialise treasury treasury snapshot from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_treasury_snapshot_init(UmiTreasuryTreasurySnapshot *value,
    const char *id,
    int64_t cash_minor,
    int64_t liquidity_gap_minor,
    int64_t risk_minor,
    int64_t collateral_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->cash_minor=cash_minor;
    value->liquidity_gap_minor=liquidity_gap_minor;
    value->risk_minor=risk_minor;
    value->collateral_minor=collateral_minor;
    return umi_treasury_treasury_snapshot_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury treasury snapshot satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_treasury_snapshot_valid(const UmiTreasuryTreasurySnapshot *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->collateral_minor >= 0);
}

/*
 * Provide the treasury treasury snapshot net liquidity minor operation used by this module
 * and its client applications.
 */
int64_t umi_treasury_treasury_snapshot_net_liquidity_minor(const UmiTreasuryTreasurySnapshot *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->cash_minor + value->liquidity_gap_minor;
}
