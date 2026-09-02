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
/**
 * Represent the workbench context event source registry data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextEventSourceRegistry {
    UmiWorkbenchContextEventSourceDescriptor *items;
    size_t count;
    size_t capacity;
    uint64_t revision;
} UmiWorkbenchContextEventSourceRegistry;
/**
 * Initialise workbench context event source registry from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_event_source_registry_init(UmiWorkbenchContextEventSourceRegistry *registry);
/**
 * Release or reset state held by workbench context event source registry so the same
 * storage can be reused safely.
 */
void umi_workbench_context_event_source_registry_destroy(UmiWorkbenchContextEventSourceRegistry *registry);
/**
 * Provide the workbench context event source registry upsert operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_event_source_registry_upsert(
    UmiWorkbenchContextEventSourceRegistry *registry,
    const UmiWorkbenchContextEventSourceDescriptor *descriptor);
/**
 * Find workbench context event source registry while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiWorkbenchContextEventSourceDescriptor *
umi_workbench_context_event_source_registry_find(
    const UmiWorkbenchContextEventSourceRegistry *registry,const char *source_id);
/**
 * Remove workbench context event source registry while keeping the remaining records in a
 * valid and discoverable state.
 */
UmiStatus umi_workbench_context_event_source_registry_remove(
    UmiWorkbenchContextEventSourceRegistry *registry,const char *source_id);
#ifdef __cplusplus
}
#endif
#endif
