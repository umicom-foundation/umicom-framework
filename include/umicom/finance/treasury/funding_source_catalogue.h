/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/funding_source_catalogue.h
 *
 * PURPOSE:
 *   Catalogue reusable funding facilities and prevent duplicate identifiers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_FUNDING_SOURCE_CATALOGUE_H
#define UMICOM_FINANCE_TREASURY_FUNDING_SOURCE_CATALOGUE_H
#include "umicom/finance/treasury/funding_source.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury funding source catalogue data shared with callers of this public
 * contract.
 */
typedef struct UmiTreasuryFundingSourceCatalogue {
    UmiTreasuryFundingSource items[UMI_TREASURY_MAX_ITEMS];
    size_t count;
} UmiTreasuryFundingSourceCatalogue;
/**
 * Initialise treasury funding source catalogue from caller-provided values so later
 * operations receive a known state.
 */
void umi_treasury_funding_source_catalogue_init(UmiTreasuryFundingSourceCatalogue *registry);
/**
 * Add treasury funding source catalogue only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_treasury_funding_source_catalogue_add(UmiTreasuryFundingSourceCatalogue *registry, const UmiTreasuryFundingSource *item);
/**
 * Find treasury funding source catalogue while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiTreasuryFundingSource *umi_treasury_funding_source_catalogue_find(const UmiTreasuryFundingSourceCatalogue *registry, const char *id);
#ifdef __cplusplus
}
#endif
#endif
