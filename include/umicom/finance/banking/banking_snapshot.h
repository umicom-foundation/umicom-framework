/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/banking/banking_snapshot.h
 *
 * PURPOSE:
 *   Capture aggregate customer, deposit and lending counts and balances for operational views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_BANKING_BANKING_SNAPSHOT_H
#define UMICOM_FINANCE_BANKING_BANKING_SNAPSHOT_H
#include "umicom/finance/banking/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the banking banking snapshot data shared with callers of this public contract.
 */
typedef struct UmiBankingBankingSnapshot {
    UmiFinancialId id;
    size_t customer_count;
    size_t deposit_account_count;
    size_t loan_count;
    int64_t deposit_balance_minor;
    int64_t loan_balance_minor;
} UmiBankingBankingSnapshot;
/**
 * Initialise banking banking snapshot from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_banking_banking_snapshot_init(UmiBankingBankingSnapshot *value,
    const char *id,
    size_t customer_count,
    size_t deposit_account_count,
    size_t loan_count,
    int64_t deposit_balance_minor,
    int64_t loan_balance_minor);
/**
 * Check that banking banking snapshot satisfies its contract before another service relies
 * on it.
 */
bool umi_banking_banking_snapshot_valid(const UmiBankingBankingSnapshot *value);
/**
 * Provide the banking banking snapshot net funding minor operation used by this module and
 * its client applications.
 */
int64_t umi_banking_banking_snapshot_net_funding_minor(const UmiBankingBankingSnapshot *value);
#ifdef __cplusplus
}
#endif
#endif
