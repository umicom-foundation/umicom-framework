/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/custody_account.h
 *
 * PURPOSE:
 *   Model a securities custody account and segregation status.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_CUSTODY_ACCOUNT_H
#define UMICOM_FINANCE_TREASURY_CUSTODY_ACCOUNT_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury custody account data shared with callers of this public contract.
 */
typedef struct UmiTreasuryCustodyAccount {
    char id[UMI_TREASURY_ID_CAPACITY];
    char custodian_id[UMI_TREASURY_ID_CAPACITY];
    bool segregated;
} UmiTreasuryCustodyAccount;
/**
 * Initialise treasury custody account from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_custody_account_init(UmiTreasuryCustodyAccount *value,
    const char *id,
    const char *custodian_id,
    bool segregated);
/**
 * Check that treasury custody account satisfies its contract before another service relies
 * on it.
 */
bool umi_treasury_custody_account_valid(const UmiTreasuryCustodyAccount *value);
/**
 * Provide the treasury custody account is segregated operation used by this module and its
 * client applications.
 */
bool umi_treasury_custody_account_is_segregated(const UmiTreasuryCustodyAccount *value);
#ifdef __cplusplus
}
#endif
#endif
