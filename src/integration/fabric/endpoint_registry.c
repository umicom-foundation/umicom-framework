/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/endpoint_registry.c
 *
 * PURPOSE:
 *   Maintain unique endpoint definitions used by service instances and connectors.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/endpoint_registry.h"
#include <string.h>
#include <limits.h>

/*
 * Initialise fabric endpoint registry from caller-provided values so later operations
 * receive a known state.
 */
void umi_fabric_endpoint_registry_init(UmiFabricEndpointRegistry *registry) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (registry!=NULL) (void)memset(registry,0,sizeof(*registry)); }
/*
 * Add fabric endpoint registry only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_fabric_endpoint_registry_add(UmiFabricEndpointRegistry *registry,const UmiFabricEndpointDescriptor *item) {
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry==NULL || item==NULL || item->endpoint_id[0]=='\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i=0U;i<registry->count;++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(registry->items[i].endpoint_id,item->endpoint_id)==0) return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count>=UMI_FABRIC_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    registry->items[registry->count++]=*item; registry->revision++;
    return UMI_STATUS_OK;
}
/*
 * Find fabric endpoint registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_fabric_endpoint_registry_find(const UmiFabricEndpointRegistry *registry,const char *id,UmiFabricEndpointDescriptor *out_item) {
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry==NULL || id==NULL || out_item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i=0U;i<registry->count;++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(registry->items[i].endpoint_id,id)==0) { *out_item=registry->items[i]; return UMI_STATUS_OK; }
    return UMI_STATUS_NOT_FOUND;
}
/*
 * Return the number of records represented by fabric endpoint registry without changing
 * their state.
 */
size_t umi_fabric_endpoint_registry_count(const UmiFabricEndpointRegistry *registry) { return registry!=NULL?registry->count:0U; }
