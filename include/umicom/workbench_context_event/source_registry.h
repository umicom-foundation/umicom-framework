/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/source_registry.h
 *
 * PURPOSE:
 *   Register real workbench interaction sources and their default context/publication policies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_SOURCE_REGISTRY_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_SOURCE_REGISTRY_H
#include "umicom/workbench_context_event/source_descriptor.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiWorkbenchContextEventSourceRegistry {
    UmiWorkbenchContextEventSourceDescriptor *items;
    size_t count;
    size_t capacity;
    uint64_t revision;
} UmiWorkbenchContextEventSourceRegistry;
void umi_workbench_context_event_source_registry_init(UmiWorkbenchContextEventSourceRegistry *registry);
void umi_workbench_context_event_source_registry_destroy(UmiWorkbenchContextEventSourceRegistry *registry);
UmiStatus umi_workbench_context_event_source_registry_upsert(
    UmiWorkbenchContextEventSourceRegistry *registry,
    const UmiWorkbenchContextEventSourceDescriptor *descriptor);
const UmiWorkbenchContextEventSourceDescriptor *
umi_workbench_context_event_source_registry_find(
    const UmiWorkbenchContextEventSourceRegistry *registry,const char *source_id);
UmiStatus umi_workbench_context_event_source_registry_remove(
    UmiWorkbenchContextEventSourceRegistry *registry,const char *source_id);
#ifdef __cplusplus
}
#endif
#endif
