/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/repo_contract.h
 *
 * PURPOSE:
 *   Model repo cash principal, collateral value and repo rate.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_REPO_CONTRACT_H
#define UMICOM_FINANCE_TREASURY_REPO_CONTRACT_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury repo contract data shared with callers of this public contract.
 */
typedef struct UmiTreasuryRepoContract {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t cash_principal_minor;
    int64_t collateral_value_minor;
    uint32_t repo_rate_bps;
} UmiTreasuryRepoContract;
/**
 * Initialise treasury repo contract from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_repo_contract_init(UmiTreasuryRepoContract *value,
    const char *id,
    int64_t cash_principal_minor,
    int64_t collateral_value_minor,
    uint32_t repo_rate_bps);
/**
 * Check that treasury repo contract satisfies its contract before another service relies
 * on it.
 */
bool umi_treasury_repo_contract_valid(const UmiTreasuryRepoContract *value);
/**
 * Provide the treasury repo contract haircut minor operation used by this module and its
 * client applications.
 */
int64_t umi_treasury_repo_contract_haircut_minor(const UmiTreasuryRepoContract *value);
#ifdef __cplusplus
}
#endif
#endif
