/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/rate_index_catalogue.h
 *
 * PURPOSE:
 *   Provide rate-index lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_RATE_INDEX_CATALOGUE_H
#define UMICOM_FINANCE_CORE_RATE_INDEX_CATALOGUE_H

#include "umicom/finance/core/rate_index.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRateIndexCatalogue { UmiRateIndex items[UMI_FINANCIAL_CORE_MAX_ITEMS]; size_t count; } UmiRateIndexCatalogue;
/* Reset the bounded registry. */ void umi_rate_index_catalogue_init(UmiRateIndexCatalogue *registry);
/* Add a unique item. */ UmiStatus umi_rate_index_catalogue_add(UmiRateIndexCatalogue *registry,const UmiRateIndex *item);
/* Find an item by identifier. */ const UmiRateIndex *umi_rate_index_catalogue_find(const UmiRateIndexCatalogue *registry,const char *id);
#ifdef __cplusplus
}
#endif

#endif
