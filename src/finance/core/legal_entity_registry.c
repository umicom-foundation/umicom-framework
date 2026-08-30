/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/legal_entity_registry.c
 *
 * PURPOSE:
 *   Implement legal-entity lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/legal_entity_registry.h"

#include <string.h>
/* Reset the bounded registry. */ void umi_legal_entity_registry_init(UmiLegalEntityRegistry *registry){if(registry!=NULL)memset(registry,0,sizeof *registry);}
/* Find an item by identifier. */ const UmiLegalEntity *umi_legal_entity_registry_find(const UmiLegalEntityRegistry *registry,const char *id){size_t i;if(registry==NULL||id==NULL)return NULL;for(i=0U;i<registry->count;i++)if(strcmp(registry->items[i].entity_id.value,id)==0)return &registry->items[i];return NULL;}
/* Add a unique item. */ UmiStatus umi_legal_entity_registry_add(UmiLegalEntityRegistry *registry,const UmiLegalEntity *item){if(registry==NULL||item==NULL||!umi_legal_entity_is_valid(item))return UMI_STATUS_INVALID_ARGUMENT;if(umi_legal_entity_registry_find(registry,item->entity_id.value)!=NULL)return UMI_STATUS_ALREADY_EXISTS;if(registry->count>=UMI_FINANCIAL_CORE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;registry->items[registry->count++]=*item;return UMI_STATUS_OK;}
