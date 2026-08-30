/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/connector_catalogue.c
 *
 * PURPOSE:
 *   Maintain unique connector descriptors so applications consume Framework-owned adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/connector_catalogue.h"
#include <string.h>
#include <limits.h>

void umi_fabric_connector_catalogue_init(UmiFabricConnectorCatalogue *registry) { if (registry!=NULL) (void)memset(registry,0,sizeof(*registry)); }
UmiStatus umi_fabric_connector_catalogue_add(UmiFabricConnectorCatalogue *registry,const UmiFabricConnectorDescriptor *item) {
    size_t i;
    if (registry==NULL || item==NULL || item->connector_id[0]=='\0') return UMI_STATUS_INVALID_ARGUMENT;
    for (i=0U;i<registry->count;++i) if (strcmp(registry->items[i].connector_id,item->connector_id)==0) return UMI_STATUS_ALREADY_EXISTS;
    if (registry->count>=UMI_FABRIC_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    registry->items[registry->count++]=*item; registry->revision++;
    return UMI_STATUS_OK;
}
UmiStatus umi_fabric_connector_catalogue_find(const UmiFabricConnectorCatalogue *registry,const char *id,UmiFabricConnectorDescriptor *out_item) {
    size_t i;
    if (registry==NULL || id==NULL || out_item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (i=0U;i<registry->count;++i) if (strcmp(registry->items[i].connector_id,id)==0) { *out_item=registry->items[i]; return UMI_STATUS_OK; }
    return UMI_STATUS_NOT_FOUND;
}
size_t umi_fabric_connector_catalogue_count(const UmiFabricConnectorCatalogue *registry) { return registry!=NULL?registry->count:0U; }
