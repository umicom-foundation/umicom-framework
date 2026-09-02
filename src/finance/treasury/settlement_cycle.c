/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/settlement_cycle.c
 *
 * PURPOSE:
 *   Implement define settlement cycle trade-date and settlement-date offsets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/settlement_cycle.h"
#include <string.h>
/*
 * Initialise treasury settlement cycle from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_settlement_cycle_init(UmiTreasurySettlementCycle *value,
    const char *id,
    int32_t settlement_days) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->settlement_days=settlement_days;
    return umi_treasury_settlement_cycle_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury settlement cycle satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_settlement_cycle_valid(const UmiTreasurySettlementCycle *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->settlement_days >= 0 && value->settlement_days <= 30);
}

/*
 * Provide the treasury settlement cycle offset days operation used by this module and its
 * client applications.
 */
int32_t umi_treasury_settlement_cycle_offset_days(const UmiTreasurySettlementCycle *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int32_t)0;
    return value->settlement_days;
}
