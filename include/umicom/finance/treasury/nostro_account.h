/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/nostro_account.h
 *
 * PURPOSE:
 *   Track nostro ledger, available and reserved cash amounts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_NOSTRO_ACCOUNT_H
#define UMICOM_FINANCE_TREASURY_NOSTRO_ACCOUNT_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury nostro account data shared with callers of this public contract.
 */
typedef struct UmiTreasuryNostroAccount {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t ledger_minor;
    int64_t reserved_minor;
} UmiTreasuryNostroAccount;
/**
 * Initialise treasury nostro account from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_nostro_account_init(UmiTreasuryNostroAccount *value,
    const char *id,
    int64_t ledger_minor,
    int64_t reserved_minor);
/**
 * Check that treasury nostro account satisfies its contract before another service relies
 * on it.
 */
bool umi_treasury_nostro_account_valid(const UmiTreasuryNostroAccount *value);
/**
 * Provide the treasury nostro account available minor operation used by this module and
 * its client applications.
 */
int64_t umi_treasury_nostro_account_available_minor(const UmiTreasuryNostroAccount *value);
#ifdef __cplusplus
}
#endif
#endif
