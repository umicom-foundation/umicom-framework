/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/funding_requirement.c
 *
 * PURPOSE:
 *   Implement calculate a funding requirement from forecast outflows and available liquidity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/funding_requirement.h"
#include <string.h>
/*
 * Initialise treasury funding requirement from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_funding_requirement_init(UmiTreasuryFundingRequirement *value,
    const char *id,
    int64_t required_liquidity_minor,
    int64_t available_liquidity_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->required_liquidity_minor=required_liquidity_minor;
    value->available_liquidity_minor=available_liquidity_minor;
    return umi_treasury_funding_requirement_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury funding requirement satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_funding_requirement_valid(const UmiTreasuryFundingRequirement *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->required_liquidity_minor >= 0 && value->available_liquidity_minor >= 0);
}

/*
 * Provide the treasury funding requirement shortfall minor operation used by this module
 * and its client applications.
 */
int64_t umi_treasury_funding_requirement_shortfall_minor(const UmiTreasuryFundingRequirement *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->required_liquidity_minor > value->available_liquidity_minor ? value->required_liquidity_minor - value->available_liquidity_minor : 0;
}
