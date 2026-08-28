/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/account_registry.h
 *
 * PURPOSE:
 *   Provide financial-account lookup.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_ACCOUNT_REGISTRY_H
#define UMICOM_FINANCE_CORE_ACCOUNT_REGISTRY_H

#include "umicom/finance/core/financial_account.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAccountRegistry { UmiFinancialCoreAccount items[UMI_FINANCIAL_CORE_MAX_ITEMS]; size_t count; } UmiAccountRegistry;
/* Reset the bounded registry. */ void umi_account_registry_init(UmiAccountRegistry *registry);
/* Add a unique item. */ UmiStatus umi_account_registry_add(UmiAccountRegistry *registry,const UmiFinancialCoreAccount *item);
/* Find an item by identifier. */ const UmiFinancialCoreAccount *umi_account_registry_find(const UmiAccountRegistry *registry,const char *id);
#ifdef __cplusplus
}
#endif

#endif
