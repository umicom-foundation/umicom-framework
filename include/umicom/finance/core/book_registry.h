/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/book_registry.h
 *
 * PURPOSE:
 *   Provide financial-book lookup.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_BOOK_REGISTRY_H
#define UMICOM_FINANCE_CORE_BOOK_REGISTRY_H

#include "umicom/finance/core/book.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiBookRegistry { UmiFinancialBook items[UMI_FINANCIAL_CORE_MAX_ITEMS]; size_t count; } UmiBookRegistry;
/* Reset the bounded registry. */ void umi_book_registry_init(UmiBookRegistry *registry);
/* Add a unique item. */ UmiStatus umi_book_registry_add(UmiBookRegistry *registry,const UmiFinancialBook *item);
/* Find an item by identifier. */ const UmiFinancialBook *umi_book_registry_find(const UmiBookRegistry *registry,const char *id);
#ifdef __cplusplus
}
#endif

#endif
