/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/route_table.c
 *
 * PURPOSE:
 *   Maintain unique routing rules whose priority is interpreted by the route matcher.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/route_table.h"
#include <string.h>
#include <limits.h>

void umi_fabric_route_table_init(UmiFabricRouteTable *registry) { if (registry!=NULL) (void)memset(registry,0,sizeof(*registry)); }
UmiStatus umi_fabric_route_table_add(UmiFabricRouteTable *registry,const UmiFabricRouteRule *item) {
    size_t i;
    if (registry==NULL || item==NULL || item->route_id[0]=='\0') return UMI_STATUS_INVALID_ARGUMENT;
    for (i=0U;i<registry->count;++i) if (strcmp(registry->items[i].route_id,item->route_id)==0) return UMI_STATUS_ALREADY_EXISTS;
    if (registry->count>=UMI_FABRIC_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    registry->items[registry->count++]=*item; registry->revision++;
    return UMI_STATUS_OK;
}
UmiStatus umi_fabric_route_table_find(const UmiFabricRouteTable *registry,const char *id,UmiFabricRouteRule *out_item) {
    size_t i;
    if (registry==NULL || id==NULL || out_item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (i=0U;i<registry->count;++i) if (strcmp(registry->items[i].route_id,id)==0) { *out_item=registry->items[i]; return UMI_STATUS_OK; }
    return UMI_STATUS_NOT_FOUND;
}
size_t umi_fabric_route_table_count(const UmiFabricRouteTable *registry) { return registry!=NULL?registry->count:0U; }
