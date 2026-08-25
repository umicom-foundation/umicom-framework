/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/repo_contract.h
 *
 * PURPOSE:
 *   Model repo cash principal, collateral value and repo rate.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_REPO_CONTRACT_H
#define UMICOM_FINANCE_TREASURY_REPO_CONTRACT_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasuryRepoContract {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t cash_principal_minor;
    int64_t collateral_value_minor;
    uint32_t repo_rate_bps;
} UmiTreasuryRepoContract;
UmiStatus umi_treasury_repo_contract_init(UmiTreasuryRepoContract *value,
    const char *id,
    int64_t cash_principal_minor,
    int64_t collateral_value_minor,
    uint32_t repo_rate_bps);
bool umi_treasury_repo_contract_valid(const UmiTreasuryRepoContract *value);
int64_t umi_treasury_repo_contract_haircut_minor(const UmiTreasuryRepoContract *value);
#ifdef __cplusplus
}
#endif
#endif
