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
typedef struct UmiTreasuryFundingSourceCatalogue {
    UmiTreasuryFundingSource items[UMI_TREASURY_MAX_ITEMS];
    size_t count;
} UmiTreasuryFundingSourceCatalogue;
void umi_treasury_funding_source_catalogue_init(UmiTreasuryFundingSourceCatalogue *registry);
UmiStatus umi_treasury_funding_source_catalogue_add(UmiTreasuryFundingSourceCatalogue *registry, const UmiTreasuryFundingSource *item);
const UmiTreasuryFundingSource *umi_treasury_funding_source_catalogue_find(const UmiTreasuryFundingSourceCatalogue *registry, const char *id);
#ifdef __cplusplus
}
#endif
#endif
