/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/custody_account.c
 *
 * PURPOSE:
 *   Implement model a securities custody account and segregation status.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/custody_account.h"
#include <string.h>
/*
 * Initialise treasury custody account from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_custody_account_init(UmiTreasuryCustodyAccount *value,
    const char *id,
    const char *custodian_id,
    bool segregated) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status=umi_treasury_id_copy(value->custodian_id,sizeof value->custodian_id,custodian_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status!=UMI_STATUS_OK)return status;
    value->segregated=segregated;
    return umi_treasury_custody_account_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury custody account satisfies its contract before another service relies
 * on it.
 */
bool umi_treasury_custody_account_valid(const UmiTreasuryCustodyAccount *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && umi_treasury_id_valid(value->custodian_id));
}

/*
 * Provide the treasury custody account is segregated operation used by this module and its
 * client applications.
 */
bool umi_treasury_custody_account_is_segregated(const UmiTreasuryCustodyAccount *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (bool)0;
    return value->segregated;
}
