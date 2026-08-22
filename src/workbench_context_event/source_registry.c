/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_event/source_registry.c
 *
 * PURPOSE:
 *   Implement bounded dynamic interaction-source registration and lookup.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_event/source_registry.h"
#include <stdlib.h>
#include <string.h>
static UmiStatus grow(UmiWorkbenchContextEventSourceRegistry *registry)
{
    UmiWorkbenchContextEventSourceDescriptor *items;
    size_t next;
    if(registry->count<registry->capacity)return UMI_STATUS_OK;
    if(registry->capacity>=UMI_WORKBENCH_CONTEXT_EVENT_MAX_SOURCES)return UMI_STATUS_CAPACITY_EXCEEDED;
    next=registry->capacity==0U?16U:registry->capacity*2U;
    if(next>UMI_WORKBENCH_CONTEXT_EVENT_MAX_SOURCES)next=UMI_WORKBENCH_CONTEXT_EVENT_MAX_SOURCES;
    items=(UmiWorkbenchContextEventSourceDescriptor*)realloc(
        registry->items,next*sizeof(registry->items[0]));
    if(items==NULL)return UMI_STATUS_OUT_OF_MEMORY;
    if(next>registry->capacity)memset(items+registry->capacity,0,
        (next-registry->capacity)*sizeof(items[0]));
    registry->items=items;registry->capacity=next;return UMI_STATUS_OK;
}
void umi_workbench_context_event_source_registry_init(UmiWorkbenchContextEventSourceRegistry *registry)
{if(registry){memset(registry,0,sizeof(*registry));registry->revision=1U;}}
void umi_workbench_context_event_source_registry_destroy(UmiWorkbenchContextEventSourceRegistry *registry)
{if(registry){free(registry->items);memset(registry,0,sizeof(*registry));}}
const UmiWorkbenchContextEventSourceDescriptor *
umi_workbench_context_event_source_registry_find(
    const UmiWorkbenchContextEventSourceRegistry *registry,const char *source_id)
{
    size_t i;if(!registry||!source_id)return NULL;
    for(i=0U;i<registry->count;++i)if(strcmp(registry->items[i].record_id,source_id)==0)return &registry->items[i];
    return NULL;
}
UmiStatus umi_workbench_context_event_source_registry_upsert(
    UmiWorkbenchContextEventSourceRegistry *registry,
    const UmiWorkbenchContextEventSourceDescriptor *descriptor)
{
    size_t i;UmiStatus s;if(!registry||!descriptor)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_workbench_context_event_source_descriptor_validate(descriptor);if(s!=UMI_STATUS_OK)return s;
    for(i=0U;i<registry->count;++i)if(strcmp(registry->items[i].record_id,descriptor->record_id)==0){
        registry->items[i]=*descriptor;++registry->revision;return UMI_STATUS_OK;}
    s=grow(registry);if(s!=UMI_STATUS_OK)return s;registry->items[registry->count++]=*descriptor;
    ++registry->revision;return UMI_STATUS_OK;
}
UmiStatus umi_workbench_context_event_source_registry_remove(
    UmiWorkbenchContextEventSourceRegistry *registry,const char *source_id)
{
    size_t i;if(!registry||!source_id)return UMI_STATUS_INVALID_ARGUMENT;
    for(i=0U;i<registry->count;++i)if(strcmp(registry->items[i].record_id,source_id)==0){
        if(i+1U<registry->count)memmove(&registry->items[i],&registry->items[i+1U],
            (registry->count-i-1U)*sizeof(registry->items[0]));
        --registry->count;memset(&registry->items[registry->count],0,sizeof(registry->items[0]));
        ++registry->revision;return UMI_STATUS_OK;}
    return UMI_STATUS_NOT_FOUND;
}
