/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/risk_factor_catalogue.h
 *
 * PURPOSE:
 *   Catalogue reusable treasury risk factors with stable identifier lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_RISK_FACTOR_CATALOGUE_H
#define UMICOM_FINANCE_TREASURY_RISK_FACTOR_CATALOGUE_H
#include "umicom/finance/treasury/risk_factor.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasuryRiskFactorCatalogue {
    UmiTreasuryRiskFactor items[UMI_TREASURY_MAX_ITEMS];
    size_t count;
} UmiTreasuryRiskFactorCatalogue;
void umi_treasury_risk_factor_catalogue_init(UmiTreasuryRiskFactorCatalogue *registry);
UmiStatus umi_treasury_risk_factor_catalogue_add(UmiTreasuryRiskFactorCatalogue *registry, const UmiTreasuryRiskFactor *item);
const UmiTreasuryRiskFactor *umi_treasury_risk_factor_catalogue_find(const UmiTreasuryRiskFactorCatalogue *registry, const char *id);
#ifdef __cplusplus
}
#endif
#endif
