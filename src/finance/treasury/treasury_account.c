/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/treasury_account.c
 *
 * PURPOSE:
 *   Implement model a treasury-managed account with currency and operating balance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/treasury_account.h"
#include <string.h>
/*
 * Initialise treasury treasury account from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_treasury_account_init(UmiTreasuryTreasuryAccount *value,
    const char *id,
    const char *currency_code,
    int64_t balance_minor,
    int64_t minimum_balance_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_treasury_currency_from_code(currency_code,&value->currency);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->balance_minor=balance_minor;
    value->minimum_balance_minor=minimum_balance_minor;
    return umi_treasury_treasury_account_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury treasury account satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_treasury_account_valid(const UmiTreasuryTreasuryAccount *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->currency.code[0] != '\0');
}

/*
 * Provide the treasury treasury account headroom minor operation used by this module and
 * its client applications.
 */
int64_t umi_treasury_treasury_account_headroom_minor(const UmiTreasuryTreasuryAccount *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->balance_minor - value->minimum_balance_minor;
}
