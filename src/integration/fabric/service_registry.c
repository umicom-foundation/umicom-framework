/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/service_registry.c
 *
 * PURPOSE:
 *   Track unique live service instances separately from immutable service contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/service_registry.h"
#include <string.h>
#include <limits.h>

void umi_fabric_service_registry_init(UmiFabricServiceRegistry *registry) { if (registry!=NULL) (void)memset(registry,0,sizeof(*registry)); }
UmiStatus umi_fabric_service_registry_add(UmiFabricServiceRegistry *registry,const UmiFabricServiceInstance *item) {
    size_t i;
    if (registry==NULL || item==NULL || item->instance_id[0]=='\0') return UMI_STATUS_INVALID_ARGUMENT;
    for (i=0U;i<registry->count;++i) if (strcmp(registry->items[i].instance_id,item->instance_id)==0) return UMI_STATUS_ALREADY_EXISTS;
    if (registry->count>=UMI_FABRIC_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    registry->items[registry->count++]=*item; registry->revision++;
    return UMI_STATUS_OK;
}
UmiStatus umi_fabric_service_registry_find(const UmiFabricServiceRegistry *registry,const char *id,UmiFabricServiceInstance *out_item) {
    size_t i;
    if (registry==NULL || id==NULL || out_item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (i=0U;i<registry->count;++i) if (strcmp(registry->items[i].instance_id,id)==0) { *out_item=registry->items[i]; return UMI_STATUS_OK; }
    return UMI_STATUS_NOT_FOUND;
}
size_t umi_fabric_service_registry_count(const UmiFabricServiceRegistry *registry) { return registry!=NULL?registry->count:0U; }
