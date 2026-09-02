/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/service_catalogue.c
 *
 * PURPOSE:
 *   Maintain unique service contracts for discovery and compatibility negotiation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/service_catalogue.h"
#include <string.h>
#include <limits.h>

/*
 * Initialise fabric service catalogue from caller-provided values so later operations
 * receive a known state.
 */
void umi_fabric_service_catalogue_init(UmiFabricServiceCatalogue *registry) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (registry!=NULL) (void)memset(registry,0,sizeof(*registry)); }
/*
 * Add fabric service catalogue only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_fabric_service_catalogue_add(UmiFabricServiceCatalogue *registry,const UmiFabricServiceDescriptor *item) {
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry==NULL || item==NULL || item->service_id[0]=='\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i=0U;i<registry->count;++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(registry->items[i].service_id,item->service_id)==0) return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count>=UMI_FABRIC_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    registry->items[registry->count++]=*item; registry->revision++;
    return UMI_STATUS_OK;
}
/*
 * Find fabric service catalogue while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_fabric_service_catalogue_find(const UmiFabricServiceCatalogue *registry,const char *id,UmiFabricServiceDescriptor *out_item) {
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry==NULL || id==NULL || out_item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i=0U;i<registry->count;++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(registry->items[i].service_id,id)==0) { *out_item=registry->items[i]; return UMI_STATUS_OK; }
    return UMI_STATUS_NOT_FOUND;
}
/*
 * Return the number of records represented by fabric service catalogue without changing
 * their state.
 */
size_t umi_fabric_service_catalogue_count(const UmiFabricServiceCatalogue *registry) { return registry!=NULL?registry->count:0U; }
