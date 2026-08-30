/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/fixing_store.h
 *
 * PURPOSE:
 *   Provide bounded fixing stores.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_FIXING_STORE_H
#define UMICOM_FINANCE_CORE_FIXING_STORE_H

#include "umicom/finance/core/fixing.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiFixingStore { UmiFixing items[UMI_FINANCIAL_CORE_MAX_ITEMS]; size_t count; } UmiFixingStore;
/* Reset collection. */ void umi_fixing_store_init(UmiFixingStore *c);
/* Append valid item. */ UmiStatus umi_fixing_store_add(UmiFixingStore *c,const UmiFixing *item);
/* Return item count. */ size_t umi_fixing_store_count(const UmiFixingStore *c);
#ifdef __cplusplus
}
#endif

#endif
