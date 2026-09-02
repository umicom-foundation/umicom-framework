/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/repo_contract.c
 *
 * PURPOSE:
 *   Implement model repo cash principal, collateral value and repo rate.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/repo_contract.h"
#include <string.h>
/*
 * Initialise treasury repo contract from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_repo_contract_init(UmiTreasuryRepoContract *value,
    const char *id,
    int64_t cash_principal_minor,
    int64_t collateral_value_minor,
    uint32_t repo_rate_bps) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->cash_principal_minor=cash_principal_minor;
    value->collateral_value_minor=collateral_value_minor;
    value->repo_rate_bps=repo_rate_bps;
    return umi_treasury_repo_contract_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury repo contract satisfies its contract before another service relies
 * on it.
 */
bool umi_treasury_repo_contract_valid(const UmiTreasuryRepoContract *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->cash_principal_minor > 0 && value->collateral_value_minor > 0 && value->repo_rate_bps <= 10000U);
}

/*
 * Provide the treasury repo contract haircut minor operation used by this module and its
 * client applications.
 */
int64_t umi_treasury_repo_contract_haircut_minor(const UmiTreasuryRepoContract *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->collateral_value_minor > value->cash_principal_minor ? value->collateral_value_minor - value->cash_principal_minor : 0;
}
