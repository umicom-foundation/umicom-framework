/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/securities_finance_exposure.c
 *
 * PURPOSE:
 *   Implement calculate secured and unsecured exposure for securities financing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/securities_finance_exposure.h"
#include <string.h>
/*
 * Initialise treasury securities finance exposure from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_treasury_securities_finance_exposure_init(UmiTreasurySecuritiesFinanceExposure *value,
    const char *id,
    int64_t exposure_minor,
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
    value->exposure_minor=exposure_minor;
    value->collateral_minor=collateral_minor;
    return umi_treasury_securities_finance_exposure_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury securities finance exposure satisfies its contract before another
 * service relies on it.
 */
bool umi_treasury_securities_finance_exposure_valid(const UmiTreasurySecuritiesFinanceExposure *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->exposure_minor >= 0 && value->collateral_minor >= 0);
}

/*
 * Provide the treasury securities finance exposure unsecured minor operation used by this
 * module and its client applications.
 */
int64_t umi_treasury_securities_finance_exposure_unsecured_minor(const UmiTreasurySecuritiesFinanceExposure *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->exposure_minor > value->collateral_minor ? value->exposure_minor - value->collateral_minor : 0;
}
