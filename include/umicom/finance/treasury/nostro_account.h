/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/nostro_account.h
 *
 * PURPOSE:
 *   Track nostro ledger, available and reserved cash amounts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_NOSTRO_ACCOUNT_H
#define UMICOM_FINANCE_TREASURY_NOSTRO_ACCOUNT_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasuryNostroAccount {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t ledger_minor;
    int64_t reserved_minor;
} UmiTreasuryNostroAccount;
UmiStatus umi_treasury_nostro_account_init(UmiTreasuryNostroAccount *value,
    const char *id,
    int64_t ledger_minor,
    int64_t reserved_minor);
bool umi_treasury_nostro_account_valid(const UmiTreasuryNostroAccount *value);
int64_t umi_treasury_nostro_account_available_minor(const UmiTreasuryNostroAccount *value);
#ifdef __cplusplus
}
#endif
#endif
