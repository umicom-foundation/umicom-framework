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
int main(void) { UmiTreasuryRiskFactorCatalogue r; UmiTreasuryRiskFactor item; umi_treasury_risk_factor_catalogue_init(&r);
    if (umi_treasury_risk_factor_init(&item, "FX-EURUSD", UMI_TREASURY_RISK_MARKET, 10) != UMI_STATUS_OK) return 1;
    if (umi_treasury_risk_factor_catalogue_add(&r,&item) != UMI_STATUS_OK) return 2;
    if (umi_treasury_risk_factor_catalogue_find(&r,item.id) == NULL) return 3;
    if (umi_treasury_risk_factor_catalogue_add(&r,&item) != UMI_STATUS_ALREADY_EXISTS) return 4;
    return 0; }
