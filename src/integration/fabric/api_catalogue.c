/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/api_catalogue.c
 *
 * PURPOSE:
 *   Maintain unique API operation contracts for gateway and connector routing.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/api_catalogue.h"
#include <string.h>
#include <limits.h>

void umi_fabric_api_catalogue_init(UmiFabricApiCatalogue *registry) { if (registry!=NULL) (void)memset(registry,0,sizeof(*registry)); }
UmiStatus umi_fabric_api_catalogue_add(UmiFabricApiCatalogue *registry,const UmiFabricApiOperation *item) {
    size_t i;
    if (registry==NULL || item==NULL || item->operation_id[0]=='\0') return UMI_STATUS_INVALID_ARGUMENT;
    for (i=0U;i<registry->count;++i) if (strcmp(registry->items[i].operation_id,item->operation_id)==0) return UMI_STATUS_ALREADY_EXISTS;
    if (registry->count>=UMI_FABRIC_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    registry->items[registry->count++]=*item; registry->revision++;
    return UMI_STATUS_OK;
}
UmiStatus umi_fabric_api_catalogue_find(const UmiFabricApiCatalogue *registry,const char *id,UmiFabricApiOperation *out_item) {
    size_t i;
    if (registry==NULL || id==NULL || out_item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (i=0U;i<registry->count;++i) if (strcmp(registry->items[i].operation_id,id)==0) { *out_item=registry->items[i]; return UMI_STATUS_OK; }
    return UMI_STATUS_NOT_FOUND;
}
size_t umi_fabric_api_catalogue_count(const UmiFabricApiCatalogue *registry) { return registry!=NULL?registry->count:0U; }
