/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/cash_position.c
 *
 * PURPOSE:
 *   Implement represent an account cash position with settled and projected balances.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/cash_position.h"
#include <string.h>
/*
 * Initialise treasury cash position from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_cash_position_init(UmiTreasuryCashPosition *value,
    const char *id,
    const char *currency_code,
    int64_t settled_minor,
    int64_t projected_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status=umi_treasury_currency_from_code(currency_code,&value->currency);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status!=UMI_STATUS_OK) return status;
    value->settled_minor=settled_minor;
    value->projected_minor=projected_minor;
    return umi_treasury_cash_position_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury cash position satisfies its contract before another service relies
 * on it.
 */
bool umi_treasury_cash_position_valid(const UmiTreasuryCashPosition *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->currency.code[0] != '\0');
}

/*
 * Provide the treasury cash position delta minor operation used by this module and its
 * client applications.
 */
int64_t umi_treasury_cash_position_delta_minor(const UmiTreasuryCashPosition *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->projected_minor - value->settled_minor;
}
