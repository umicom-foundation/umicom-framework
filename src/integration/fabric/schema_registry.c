/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/schema_registry.c
 *
 * PURPOSE:
 *   Maintain unique schema versions using a caller-defined stable schema-version identifier.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/schema_registry.h"
#include <string.h>
#include <limits.h>

void umi_fabric_schema_registry_init(UmiFabricSchemaRegistry *registry) { if (registry!=NULL) (void)memset(registry,0,sizeof(*registry)); }
UmiStatus umi_fabric_schema_registry_add(UmiFabricSchemaRegistry *registry,const UmiFabricSchemaDescriptor *item) {
    size_t i;
    if (registry==NULL || item==NULL || item->schema_id[0]=='\0') return UMI_STATUS_INVALID_ARGUMENT;
    for (i=0U;i<registry->count;++i) if (strcmp(registry->items[i].schema_id,item->schema_id)==0) return UMI_STATUS_ALREADY_EXISTS;
    if (registry->count>=UMI_FABRIC_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    registry->items[registry->count++]=*item; registry->revision++;
    return UMI_STATUS_OK;
}
UmiStatus umi_fabric_schema_registry_find(const UmiFabricSchemaRegistry *registry,const char *id,UmiFabricSchemaDescriptor *out_item) {
    size_t i;
    if (registry==NULL || id==NULL || out_item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (i=0U;i<registry->count;++i) if (strcmp(registry->items[i].schema_id,id)==0) { *out_item=registry->items[i]; return UMI_STATUS_OK; }
    return UMI_STATUS_NOT_FOUND;
}
size_t umi_fabric_schema_registry_count(const UmiFabricSchemaRegistry *registry) { return registry!=NULL?registry->count:0U; }
