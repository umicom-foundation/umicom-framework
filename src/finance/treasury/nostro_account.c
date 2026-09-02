/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/nostro_account.c
 *
 * PURPOSE:
 *   Implement track nostro ledger, available and reserved cash amounts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/nostro_account.h"
#include <string.h>
/*
 * Initialise treasury nostro account from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_nostro_account_init(UmiTreasuryNostroAccount *value,
    const char *id,
    int64_t ledger_minor,
    int64_t reserved_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->ledger_minor=ledger_minor;
    value->reserved_minor=reserved_minor;
    return umi_treasury_nostro_account_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury nostro account satisfies its contract before another service relies
 * on it.
 */
bool umi_treasury_nostro_account_valid(const UmiTreasuryNostroAccount *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->reserved_minor >= 0);
}

/*
 * Provide the treasury nostro account available minor operation used by this module and
 * its client applications.
 */
int64_t umi_treasury_nostro_account_available_minor(const UmiTreasuryNostroAccount *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->ledger_minor - value->reserved_minor;
}
