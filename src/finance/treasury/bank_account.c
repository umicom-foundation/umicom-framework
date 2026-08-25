/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/bank_account.c
 *
 * PURPOSE:
 *   Implement model external bank account routing and currency ownership for treasury.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/bank_account.h"
#include <string.h>
UmiStatus umi_treasury_bank_account_init(UmiTreasuryBankAccount *value,
    const char *id,
    const char *bank_id,
    const char *currency_code,
    bool active) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    if (status != UMI_STATUS_OK) return status;
    status=umi_treasury_id_copy(value->bank_id,sizeof value->bank_id,bank_id);
    if(status!=UMI_STATUS_OK)return status;
    status=umi_treasury_currency_from_code(currency_code,&value->currency);
    if(status!=UMI_STATUS_OK)return status;
    value->active=active;
    return umi_treasury_bank_account_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_treasury_bank_account_valid(const UmiTreasuryBankAccount *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && umi_treasury_id_valid(value->bank_id) && value->currency.code[0] != '\0');
}

bool umi_treasury_bank_account_usable(const UmiTreasuryBankAccount *value) {
    if (value == NULL) return (bool)0;
    return value->active;
}
