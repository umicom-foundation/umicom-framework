/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/inbox_registry.c
 *
 * PURPOSE:
 *   Implement dynamic endpoint-addressable delivery inbox registration.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/inbox_registry.h"
#include <stdlib.h>
#include <string.h>
static UmiStatus grow(UmiWorkbenchContextHostInboxRegistry *registry)
{
    size_t next=registry->capacity==0U?8U:registry->capacity*2U;
    UmiWorkbenchContextHostInbox *items;
    if(registry->count<registry->capacity)return UMI_STATUS_OK;
    if(next>UMI_WORKBENCH_CONTEXT_HOST_MAX_ENDPOINTS)
        next=UMI_WORKBENCH_CONTEXT_HOST_MAX_ENDPOINTS;
    if(next<=registry->capacity)return UMI_STATUS_CAPACITY_EXCEEDED;
    items=(UmiWorkbenchContextHostInbox*)realloc(
        registry->items,next*sizeof(registry->items[0]));
    if(!items)return UMI_STATUS_OUT_OF_MEMORY;
    memset(items+registry->capacity,0,(next-registry->capacity)*sizeof(items[0]));
    registry->items=items;registry->capacity=next;return UMI_STATUS_OK;
}
void umi_workbench_context_host_inbox_registry_init(
    UmiWorkbenchContextHostInboxRegistry *registry)
{if(registry){memset(registry,0,sizeof(*registry));registry->revision=1U;}}
void umi_workbench_context_host_inbox_registry_destroy(
    UmiWorkbenchContextHostInboxRegistry *registry)
{
    size_t i;if(!registry)return;
    for(i=0U;i<registry->count;++i)umi_workbench_context_host_inbox_destroy(&registry->items[i]);
    free(registry->items);memset(registry,0,sizeof(*registry));
}
UmiWorkbenchContextHostInbox *umi_workbench_context_host_inbox_registry_find(
    UmiWorkbenchContextHostInboxRegistry *registry,const char *endpoint_id)
{
    size_t i;if(!registry||!endpoint_id)return NULL;
    for(i=0U;i<registry->count;++i)if(strcmp(registry->items[i].endpoint_id,endpoint_id)==0)
        return &registry->items[i];
    return NULL;
}
const UmiWorkbenchContextHostInbox *umi_workbench_context_host_inbox_registry_find_const(
    const UmiWorkbenchContextHostInboxRegistry *registry,const char *endpoint_id)
{
    size_t i;if(!registry||!endpoint_id)return NULL;
    for(i=0U;i<registry->count;++i)if(strcmp(registry->items[i].endpoint_id,endpoint_id)==0)
        return &registry->items[i];
    return NULL;
}
UmiStatus umi_workbench_context_host_inbox_registry_ensure(
    UmiWorkbenchContextHostInboxRegistry *registry,
    const char *endpoint_id,const char *panel_id,UmiWorkbenchContextHostInbox **out_inbox)
{
    UmiWorkbenchContextHostInbox *inbox;UmiStatus s;
    if(!registry||!endpoint_id||!panel_id||!out_inbox)return UMI_STATUS_INVALID_ARGUMENT;
    inbox=umi_workbench_context_host_inbox_registry_find(registry,endpoint_id);
    if(inbox){*out_inbox=inbox;return UMI_STATUS_OK;}
    s=grow(registry);if(s!=UMI_STATUS_OK)return s;
    inbox=&registry->items[registry->count++];
    umi_workbench_context_host_inbox_init(inbox,endpoint_id,panel_id);
    ++registry->revision;*out_inbox=inbox;return UMI_STATUS_OK;
}
UmiStatus umi_workbench_context_host_inbox_registry_remove(
    UmiWorkbenchContextHostInboxRegistry *registry,const char *endpoint_id)
{
    size_t i;if(!registry||!endpoint_id)return UMI_STATUS_INVALID_ARGUMENT;
    for(i=0U;i<registry->count;++i)if(strcmp(registry->items[i].endpoint_id,endpoint_id)==0){
        umi_workbench_context_host_inbox_destroy(&registry->items[i]);
        if(i+1U<registry->count)memmove(&registry->items[i],&registry->items[i+1U],
            (registry->count-i-1U)*sizeof(registry->items[0]));
        --registry->count;memset(&registry->items[registry->count],0,sizeof(registry->items[0]));
        ++registry->revision;return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_FOUND;
}
