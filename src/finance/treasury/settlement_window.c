/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/settlement_window.c
 *
 * PURPOSE:
 *   Implement define operational settlement opening and cut-off timestamps.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/settlement_window.h"
#include <string.h>
/*
 * Initialise treasury settlement window from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_settlement_window_init(UmiTreasurySettlementWindow *value,
    const char *id,
    int64_t opens_epoch_millis,
    int64_t closes_epoch_millis) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->opens_epoch_millis=opens_epoch_millis;
    value->closes_epoch_millis=closes_epoch_millis;
    return umi_treasury_settlement_window_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury settlement window satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_settlement_window_valid(const UmiTreasurySettlementWindow *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->opens_epoch_millis >= 0 && value->closes_epoch_millis >= value->opens_epoch_millis);
}

/*
 * Provide the treasury settlement window duration millis operation used by this module and
 * its client applications.
 */
int64_t umi_treasury_settlement_window_duration_millis(const UmiTreasurySettlementWindow *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->closes_epoch_millis - value->opens_epoch_millis;
}
