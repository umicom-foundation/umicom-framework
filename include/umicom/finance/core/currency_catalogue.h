/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/currency_catalogue.h
 *
 * PURPOSE:
 *   Provide a bounded currency catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_CURRENCY_CATALOGUE_H
#define UMICOM_FINANCE_CORE_CURRENCY_CATALOGUE_H

#include "umicom/finance/core/currency_descriptor.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the currency catalogue data shared with callers of this public contract.
 */
typedef struct UmiCurrencyCatalogue { UmiCurrencyDescriptor items[UMI_FINANCIAL_CORE_MAX_ITEMS]; size_t count; } UmiCurrencyCatalogue;
/* Reset the bounded registry. */ void umi_currency_catalogue_init(UmiCurrencyCatalogue *registry);
/* Add a unique item. */ UmiStatus umi_currency_catalogue_add(UmiCurrencyCatalogue *registry,const UmiCurrencyDescriptor *item);
/* Find an item by identifier. */ const UmiCurrencyDescriptor *umi_currency_catalogue_find(const UmiCurrencyCatalogue *registry,const char *id);
#ifdef __cplusplus
}
#endif

#endif
