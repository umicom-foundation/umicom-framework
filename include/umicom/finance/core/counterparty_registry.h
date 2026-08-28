/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/counterparty_registry.h
 *
 * PURPOSE:
 *   Provide counterparty lookup.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_COUNTERPARTY_REGISTRY_H
#define UMICOM_FINANCE_CORE_COUNTERPARTY_REGISTRY_H

#include "umicom/finance/core/counterparty.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiCounterpartyRegistry { UmiCounterparty items[UMI_FINANCIAL_CORE_MAX_ITEMS]; size_t count; } UmiCounterpartyRegistry;
/* Reset the bounded registry. */ void umi_counterparty_registry_init(UmiCounterpartyRegistry *registry);
/* Add a unique item. */ UmiStatus umi_counterparty_registry_add(UmiCounterpartyRegistry *registry,const UmiCounterparty *item);
/* Find an item by identifier. */ const UmiCounterparty *umi_counterparty_registry_find(const UmiCounterpartyRegistry *registry,const char *id);
#ifdef __cplusplus
}
#endif

#endif
