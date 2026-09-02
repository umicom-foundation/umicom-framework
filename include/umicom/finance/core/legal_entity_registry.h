/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/legal_entity_registry.h
 *
 * PURPOSE:
 *   Provide legal-entity lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_LEGAL_ENTITY_REGISTRY_H
#define UMICOM_FINANCE_CORE_LEGAL_ENTITY_REGISTRY_H

#include "umicom/finance/core/legal_entity.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the legal entity registry data shared with callers of this public contract.
 */
typedef struct UmiLegalEntityRegistry { UmiLegalEntity items[UMI_FINANCIAL_CORE_MAX_ITEMS]; size_t count; } UmiLegalEntityRegistry;
/* Reset the bounded registry. */ void umi_legal_entity_registry_init(UmiLegalEntityRegistry *registry);
/* Add a unique item. */ UmiStatus umi_legal_entity_registry_add(UmiLegalEntityRegistry *registry,const UmiLegalEntity *item);
/* Find an item by identifier. */ const UmiLegalEntity *umi_legal_entity_registry_find(const UmiLegalEntityRegistry *registry,const char *id);
#ifdef __cplusplus
}
#endif

#endif
