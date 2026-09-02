/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/liquidity_buffer.c
 *
 * PURPOSE:
 *   Implement compare available liquidity against a policy buffer requirement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/liquidity_buffer.h"
#include <string.h>
/*
 * Initialise treasury liquidity buffer from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_liquidity_buffer_init(UmiTreasuryLiquidityBuffer *value,
    const char *id,
    int64_t available_minor,
    int64_t required_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->available_minor=available_minor;
    value->required_minor=required_minor;
    return umi_treasury_liquidity_buffer_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury liquidity buffer satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_liquidity_buffer_valid(const UmiTreasuryLiquidityBuffer *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->available_minor >= 0 && value->required_minor >= 0);
}

/*
 * Provide the treasury liquidity buffer surplus minor operation used by this module and
 * its client applications.
 */
int64_t umi_treasury_liquidity_buffer_surplus_minor(const UmiTreasuryLiquidityBuffer *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->available_minor - value->required_minor;
}
