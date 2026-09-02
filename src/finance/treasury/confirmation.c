/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/confirmation.c
 *
 * PURPOSE:
 *   Implement represent trade confirmation terms and confirmation state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/confirmation.h"
#include <string.h>
/*
 * Initialise treasury confirmation from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_treasury_confirmation_init(UmiTreasuryConfirmation *value,
    const char *id,
    const char *trade_id,
    bool sent,
    bool acknowledged) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status=umi_treasury_id_copy(value->trade_id,sizeof value->trade_id,trade_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status!=UMI_STATUS_OK)return status;
    value->sent=sent;
    value->acknowledged=acknowledged;
    return umi_treasury_confirmation_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury confirmation satisfies its contract before another service relies on
 * it.
 */
bool umi_treasury_confirmation_valid(const UmiTreasuryConfirmation *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && umi_treasury_id_valid(value->trade_id) && (!value->acknowledged || value->sent));
}

/*
 * Provide the treasury confirmation complete operation used by this module and its client
 * applications.
 */
bool umi_treasury_confirmation_complete(const UmiTreasuryConfirmation *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (bool)0;
    return value->sent && value->acknowledged;
}
