/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/trade_registry.h
 *
 * PURPOSE:
 *   Provide trade lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_TRADE_REGISTRY_H
#define UMICOM_FINANCE_CORE_TRADE_REGISTRY_H

#include "umicom/finance/core/trade_descriptor.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTradeRegistry { UmiTradeDescriptor items[UMI_FINANCIAL_CORE_MAX_ITEMS]; size_t count; } UmiTradeRegistry;
/* Reset the bounded registry. */ void umi_trade_registry_init(UmiTradeRegistry *registry);
/* Add a unique item. */ UmiStatus umi_trade_registry_add(UmiTradeRegistry *registry,const UmiTradeDescriptor *item);
/* Find an item by identifier. */ const UmiTradeDescriptor *umi_trade_registry_find(const UmiTradeRegistry *registry,const char *id);
#ifdef __cplusplus
}
#endif

#endif
