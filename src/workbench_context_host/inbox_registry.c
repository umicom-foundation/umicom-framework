/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/inbox_registry.c
 *
 * PURPOSE:
 *   Implement dynamic endpoint-addressable delivery inbox registration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/inbox_registry.h"
#include <stdlib.h>
#include <string.h>
/* Provide the grow operation used by this module and its client applications. */
static UmiStatus grow(UmiWorkbenchContextHostInboxRegistry *registry)
{
    size_t next=registry->capacity==0U?8U:registry->capacity*2U;
    UmiWorkbenchContextHostInbox *items;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if(registry->count<registry->capacity)return UMI_STATUS_OK;
    /* Apply this branch only when its contract condition is satisfied. */
    if(next>UMI_WORKBENCH_CONTEXT_HOST_MAX_ENDPOINTS)
        next=UMI_WORKBENCH_CONTEXT_HOST_MAX_ENDPOINTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if(next<=registry->capacity)return UMI_STATUS_CAPACITY_EXCEEDED;
    items=(UmiWorkbenchContextHostInbox*)realloc(
        registry->items,next*sizeof(registry->items[0]));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!items)return UMI_STATUS_OUT_OF_MEMORY;
    memset(items+registry->capacity,0,(next-registry->capacity)*sizeof(items[0]));
    registry->items=items;registry->capacity=next;return UMI_STATUS_OK;
}
/*
 * Initialise workbench context host inbox registry from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_host_inbox_registry_init(
    UmiWorkbenchContextHostInboxRegistry *registry)
{/* Apply this branch only when its contract condition is satisfied. */ if(registry){memset(registry,0,sizeof(*registry));registry->revision=1U;}}
/*
 * Release or reset state held by workbench context host inbox registry so the same storage
 * can be reused safely.
 */
void umi_workbench_context_host_inbox_registry_destroy(
    UmiWorkbenchContextHostInboxRegistry *registry)
{
    size_t i;/* Apply this branch only when its contract condition is satisfied. */ if(!registry)return;
    /* Visit each bounded item once so every record receives the same rule. */
    for(i=0U;i<registry->count;++i)umi_workbench_context_host_inbox_destroy(&registry->items[i]);
    free(registry->items);memset(registry,0,sizeof(*registry));
}
/*
 * Find workbench context host inbox registry while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiWorkbenchContextHostInbox *umi_workbench_context_host_inbox_registry_find(
    UmiWorkbenchContextHostInboxRegistry *registry,const char *endpoint_id)
{
    size_t i;/* Use the stable identifier comparison to choose the matching record or policy. */ if(!registry||!endpoint_id)return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for(i=0U;i<registry->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(registry->items[i].endpoint_id,endpoint_id)==0)
        return &registry->items[i];
    return NULL;
}
/*
 * Provide the workbench context host inbox registry find const operation used by this
 * module and its client applications.
 */
const UmiWorkbenchContextHostInbox *umi_workbench_context_host_inbox_registry_find_const(
    const UmiWorkbenchContextHostInboxRegistry *registry,const char *endpoint_id)
{
    size_t i;/* Use the stable identifier comparison to choose the matching record or policy. */ if(!registry||!endpoint_id)return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for(i=0U;i<registry->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(registry->items[i].endpoint_id,endpoint_id)==0)
        return &registry->items[i];
    return NULL;
}
/*
 * Provide the workbench context host inbox registry ensure operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_host_inbox_registry_ensure(
    UmiWorkbenchContextHostInboxRegistry *registry,
    const char *endpoint_id,const char *panel_id,UmiWorkbenchContextHostInbox **out_inbox)
{
    UmiWorkbenchContextHostInbox *inbox;UmiStatus s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!registry||!endpoint_id||!panel_id||!out_inbox)return UMI_STATUS_INVALID_ARGUMENT;
    inbox=umi_workbench_context_host_inbox_registry_find(registry,endpoint_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(inbox){*out_inbox=inbox;return UMI_STATUS_OK;}
    s=grow(registry);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;
    inbox=&registry->items[registry->count++];
    umi_workbench_context_host_inbox_init(inbox,endpoint_id,panel_id);
    ++registry->revision;*out_inbox=inbox;return UMI_STATUS_OK;
}
/*
 * Remove workbench context host inbox registry while keeping the remaining records in a
 * valid and discoverable state.
 */
UmiStatus umi_workbench_context_host_inbox_registry_remove(
    UmiWorkbenchContextHostInboxRegistry *registry,const char *endpoint_id)
{
    size_t i;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!registry||!endpoint_id)return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for(i=0U;i<registry->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(registry->items[i].endpoint_id,endpoint_id)==0){
        umi_workbench_context_host_inbox_destroy(&registry->items[i]);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if(i+1U<registry->count)memmove(&registry->items[i],&registry->items[i+1U],
            (registry->count-i-1U)*sizeof(registry->items[0]));
        --registry->count;memset(&registry->items[registry->count],0,sizeof(registry->items[0]));
        ++registry->revision;return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_FOUND;
}
