/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/funding_requirement.c
 *
 * PURPOSE:
 *   Implement calculate a funding requirement from forecast outflows and available liquidity.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/funding_requirement.h"
#include <string.h>
UmiStatus umi_treasury_funding_requirement_init(UmiTreasuryFundingRequirement *value,
    const char *id,
    int64_t required_liquidity_minor,
    int64_t available_liquidity_minor) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    if (status != UMI_STATUS_OK) return status;
    value->required_liquidity_minor=required_liquidity_minor;
    value->available_liquidity_minor=available_liquidity_minor;
    return umi_treasury_funding_requirement_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_treasury_funding_requirement_valid(const UmiTreasuryFundingRequirement *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->required_liquidity_minor >= 0 && value->available_liquidity_minor >= 0);
}

int64_t umi_treasury_funding_requirement_shortfall_minor(const UmiTreasuryFundingRequirement *value) {
    if (value == NULL) return (int64_t)0;
    return value->required_liquidity_minor > value->available_liquidity_minor ? value->required_liquidity_minor - value->available_liquidity_minor : 0;
}
