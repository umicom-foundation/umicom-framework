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
/**
 * Represent the treasury risk factor catalogue data shared with callers of this public
 * contract.
 */
typedef struct UmiTreasuryRiskFactorCatalogue {
    UmiTreasuryRiskFactor items[UMI_TREASURY_MAX_ITEMS];
    size_t count;
} UmiTreasuryRiskFactorCatalogue;
/**
 * Initialise treasury risk factor catalogue from caller-provided values so later
 * operations receive a known state.
 */
void umi_treasury_risk_factor_catalogue_init(UmiTreasuryRiskFactorCatalogue *registry);
/**
 * Add treasury risk factor catalogue only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_treasury_risk_factor_catalogue_add(UmiTreasuryRiskFactorCatalogue *registry, const UmiTreasuryRiskFactor *item);
/**
 * Find treasury risk factor catalogue while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiTreasuryRiskFactor *umi_treasury_risk_factor_catalogue_find(const UmiTreasuryRiskFactorCatalogue *registry, const char *id);
#ifdef __cplusplus
}
#endif
#endif
