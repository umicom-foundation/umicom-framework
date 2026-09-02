/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/collateral_valuation.c
 *
 * PURPOSE:
 *   Implement calculate post-haircut collateral value.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/collateral_valuation.h"
#include <string.h>
/*
 * Initialise treasury collateral valuation from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_collateral_valuation_init(UmiTreasuryCollateralValuation *value,
    const char *id,
    int64_t gross_value_minor,
    uint32_t haircut_bps) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->gross_value_minor=gross_value_minor;
    value->haircut_bps=haircut_bps;
    return umi_treasury_collateral_valuation_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury collateral valuation satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_collateral_valuation_valid(const UmiTreasuryCollateralValuation *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->gross_value_minor >= 0 && value->haircut_bps <= 10000U);
}

/*
 * Provide the treasury collateral valuation eligible value minor operation used by this
 * module and its client applications.
 */
int64_t umi_treasury_collateral_valuation_eligible_value_minor(const UmiTreasuryCollateralValuation *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return (value->gross_value_minor * (int64_t)(10000U - value->haircut_bps)) / 10000;
}
