/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_event/source_registry.c
 *
 * PURPOSE:
 *   Implement bounded dynamic interaction-source registration and lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_event/source_registry.h"
#include <stdlib.h>
#include <string.h>
/* Provide the grow operation used by this module and its client applications. */
static UmiStatus grow(UmiWorkbenchContextEventSourceRegistry *registry)
{
    UmiWorkbenchContextEventSourceDescriptor *items;
    size_t next;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if(registry->count<registry->capacity)return UMI_STATUS_OK;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if(registry->capacity>=UMI_WORKBENCH_CONTEXT_EVENT_MAX_SOURCES)return UMI_STATUS_CAPACITY_EXCEEDED;
    next=registry->capacity==0U?16U:registry->capacity*2U;
    /* Apply this branch only when its contract condition is satisfied. */
    if(next>UMI_WORKBENCH_CONTEXT_EVENT_MAX_SOURCES)next=UMI_WORKBENCH_CONTEXT_EVENT_MAX_SOURCES;
    items=(UmiWorkbenchContextEventSourceDescriptor*)realloc(
        registry->items,next*sizeof(registry->items[0]));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(items==NULL)return UMI_STATUS_OUT_OF_MEMORY;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if(next>registry->capacity)memset(items+registry->capacity,0,
        (next-registry->capacity)*sizeof(items[0]));
    registry->items=items;registry->capacity=next;return UMI_STATUS_OK;
}
/*
 * Initialise workbench context event source registry from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_event_source_registry_init(UmiWorkbenchContextEventSourceRegistry *registry)
{/* Apply this branch only when its contract condition is satisfied. */ if(registry){memset(registry,0,sizeof(*registry));registry->revision=1U;}}
/*
 * Release or reset state held by workbench context event source registry so the same
 * storage can be reused safely.
 */
void umi_workbench_context_event_source_registry_destroy(UmiWorkbenchContextEventSourceRegistry *registry)
{/* Apply this branch only when its contract condition is satisfied. */ if(registry){free(registry->items);memset(registry,0,sizeof(*registry));}}
/*
 * Find workbench context event source registry while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiWorkbenchContextEventSourceDescriptor *
umi_workbench_context_event_source_registry_find(
    const UmiWorkbenchContextEventSourceRegistry *registry,const char *source_id)
{
    size_t i;/* Use the stable identifier comparison to choose the matching record or policy. */ if(!registry||!source_id)return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for(i=0U;i<registry->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(registry->items[i].record_id,source_id)==0)return &registry->items[i];
    return NULL;
}
/*
 * Provide the workbench context event source registry upsert operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_event_source_registry_upsert(
    UmiWorkbenchContextEventSourceRegistry *registry,
    const UmiWorkbenchContextEventSourceDescriptor *descriptor)
{
    size_t i;UmiStatus s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!registry||!descriptor)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_workbench_context_event_source_descriptor_validate(descriptor);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;
    /* Visit each bounded item once so every record receives the same rule. */
    for(i=0U;i<registry->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(registry->items[i].record_id,descriptor->record_id)==0){
        registry->items[i]=*descriptor;++registry->revision;return UMI_STATUS_OK;}
    s=grow(registry);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s!=UMI_STATUS_OK)return s;registry->items[registry->count++]=*descriptor;
    ++registry->revision;return UMI_STATUS_OK;
}
/*
 * Remove workbench context event source registry while keeping the remaining records in a
 * valid and discoverable state.
 */
UmiStatus umi_workbench_context_event_source_registry_remove(
    UmiWorkbenchContextEventSourceRegistry *registry,const char *source_id)
{
    size_t i;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!registry||!source_id)return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for(i=0U;i<registry->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(registry->items[i].record_id,source_id)==0){
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if(i+1U<registry->count)memmove(&registry->items[i],&registry->items[i+1U],
            (registry->count-i-1U)*sizeof(registry->items[0]));
        --registry->count;memset(&registry->items[registry->count],0,sizeof(registry->items[0]));
        ++registry->revision;return UMI_STATUS_OK;}
    return UMI_STATUS_NOT_FOUND;
}
