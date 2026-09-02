/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/settlement_obligation.c
 *
 * PURPOSE:
 *   Implement represent delivery-versus-payment settlement obligations and state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/settlement_obligation.h"
#include <string.h>
/*
 * Initialise treasury settlement obligation from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_treasury_settlement_obligation_init(UmiTreasurySettlementObligation *value,
    const char *id,
    int64_t cash_minor,
    int64_t security_quantity,
    UmiTreasurySettlementState state) {
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
    value->security_quantity=security_quantity;
    value->state=state;
    return umi_treasury_settlement_obligation_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury settlement obligation satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_settlement_obligation_valid(const UmiTreasurySettlementObligation *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->cash_minor >= 0 && value->security_quantity >= 0 && value->state >= UMI_TREASURY_SETTLEMENT_NEW && value->state <= UMI_TREASURY_SETTLEMENT_CANCELLED);
}

/*
 * Provide the treasury settlement obligation complete operation used by this module and
 * its client applications.
 */
bool umi_treasury_settlement_obligation_complete(const UmiTreasurySettlementObligation *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (bool)0;
    return value->state == UMI_TREASURY_SETTLEMENT_SETTLED;
}
