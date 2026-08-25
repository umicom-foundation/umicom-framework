/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/custody_account.h
 *
 * PURPOSE:
 *   Model a securities custody account and segregation status.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_CUSTODY_ACCOUNT_H
#define UMICOM_FINANCE_TREASURY_CUSTODY_ACCOUNT_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasuryCustodyAccount {
    char id[UMI_TREASURY_ID_CAPACITY];
    char custodian_id[UMI_TREASURY_ID_CAPACITY];
    bool segregated;
} UmiTreasuryCustodyAccount;
UmiStatus umi_treasury_custody_account_init(UmiTreasuryCustodyAccount *value,
    const char *id,
    const char *custodian_id,
    bool segregated);
bool umi_treasury_custody_account_valid(const UmiTreasuryCustodyAccount *value);
bool umi_treasury_custody_account_is_segregated(const UmiTreasuryCustodyAccount *value);
#ifdef __cplusplus
}
#endif
#endif
