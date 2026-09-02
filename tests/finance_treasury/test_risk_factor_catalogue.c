/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_risk_factor_catalogue.c
 *
 * PURPOSE:
 *   Verify bounded risk factor catalogue registration and lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/risk_factor_catalogue.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiTreasuryRiskFactorCatalogue r; UmiTreasuryRiskFactor item; umi_treasury_risk_factor_catalogue_init(&r);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_risk_factor_init(&item, "FX-EURUSD", UMI_TREASURY_RISK_MARKET, 10) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_risk_factor_catalogue_add(&r,&item) != UMI_STATUS_OK) return 2;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_treasury_risk_factor_catalogue_find(&r,item.id) == NULL) return 3;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_risk_factor_catalogue_add(&r,&item) != UMI_STATUS_ALREADY_EXISTS) return 4;
    return 0; }
