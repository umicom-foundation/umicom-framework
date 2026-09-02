/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/securities_lending_contract.c
 *
 * PURPOSE:
 *   Implement model securities lending quantity, collateral value and fee rate.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/securities_lending_contract.h"
#include <string.h>
/*
 * Initialise treasury securities lending contract from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_treasury_securities_lending_contract_init(UmiTreasurySecuritiesLendingContract *value,
    const char *id,
    int64_t quantity,
    int64_t collateral_minor,
    uint32_t fee_bps) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->quantity=quantity;
    value->collateral_minor=collateral_minor;
    value->fee_bps=fee_bps;
    return umi_treasury_securities_lending_contract_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury securities lending contract satisfies its contract before another
 * service relies on it.
 */
bool umi_treasury_securities_lending_contract_valid(const UmiTreasurySecuritiesLendingContract *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->quantity > 0 && value->collateral_minor >= 0 && value->fee_bps <= 10000U);
}

/*
 * Provide the treasury securities lending contract annual fee minor operation used by this
 * module and its client applications.
 */
int64_t umi_treasury_securities_lending_contract_annual_fee_minor(const UmiTreasurySecuritiesLendingContract *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return (value->collateral_minor * (int64_t)value->fee_bps) / 10000;
}
