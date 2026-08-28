/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/product_registry.h
 *
 * PURPOSE:
 *   Provide product lookup.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_PRODUCT_REGISTRY_H
#define UMICOM_FINANCE_CORE_PRODUCT_REGISTRY_H

#include "umicom/finance/core/product_descriptor.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiProductRegistry { UmiProductDescriptor items[UMI_FINANCIAL_CORE_MAX_ITEMS]; size_t count; } UmiProductRegistry;
/* Reset the bounded registry. */ void umi_product_registry_init(UmiProductRegistry *registry);
/* Add a unique item. */ UmiStatus umi_product_registry_add(UmiProductRegistry *registry,const UmiProductDescriptor *item);
/* Find an item by identifier. */ const UmiProductDescriptor *umi_product_registry_find(const UmiProductRegistry *registry,const char *id);
#ifdef __cplusplus
}
#endif

#endif
