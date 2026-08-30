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
UmiStatus umi_treasury_treasury_account_init(UmiTreasuryTreasuryAccount *value,
    const char *id,
    const char *currency_code,
    int64_t balance_minor,
    int64_t minimum_balance_minor) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_treasury_currency_from_code(currency_code,&value->currency);
    if (status != UMI_STATUS_OK) return status;
    value->balance_minor=balance_minor;
    value->minimum_balance_minor=minimum_balance_minor;
    return umi_treasury_treasury_account_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_treasury_treasury_account_valid(const UmiTreasuryTreasuryAccount *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->currency.code[0] != '\0');
}

int64_t umi_treasury_treasury_account_headroom_minor(const UmiTreasuryTreasuryAccount *value) {
    if (value == NULL) return (int64_t)0;
    return value->balance_minor - value->minimum_balance_minor;
}
