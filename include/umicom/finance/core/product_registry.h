/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/product_registry.h
 *
 * PURPOSE:
 *   Provide product lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_PRODUCT_REGISTRY_H
#define UMICOM_FINANCE_CORE_PRODUCT_REGISTRY_H

#include "umicom/finance/core/product_descriptor.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the financial product registry data shared with callers of this public
 * contract.
 */
typedef struct UmiFinancialProductRegistry {
  UmiProductDescriptor items[UMI_FINANCIAL_CORE_MAX_ITEMS];
  size_t count;
} UmiFinancialProductRegistry;
/* Reset the bounded registry. */ void
umi_financial_product_registry_init(UmiFinancialProductRegistry *registry);
/* Add a unique item. */ UmiStatus
umi_financial_product_registry_add(UmiFinancialProductRegistry *registry,
                                   const UmiProductDescriptor *item);
/* Find an item by identifier. */ const UmiProductDescriptor *
umi_financial_product_registry_find(const UmiFinancialProductRegistry *registry, const char *id);
#ifdef __cplusplus
}
#endif

#endif
