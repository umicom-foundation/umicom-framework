/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/registry.h
 *
 * PURPOSE:
 *   Maintain configured live interaction sources with bounded dynamic storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_REGISTRY_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_REGISTRY_H

#include "umicom/workbench_context_source/definition.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceRegistry {
    UmiWorkbenchContextSourceDefinition *items;
    size_t count;
    size_t capacity;
    uint64_t revision;
} UmiWorkbenchContextSourceRegistry;

void umi_workbench_context_source_registry_init(
    UmiWorkbenchContextSourceRegistry *registry);
void umi_workbench_context_source_registry_destroy(
    UmiWorkbenchContextSourceRegistry *registry);
UmiStatus umi_workbench_context_source_registry_upsert(
    UmiWorkbenchContextSourceRegistry *registry,
    const UmiWorkbenchContextSourceDefinition *definition);
const UmiWorkbenchContextSourceDefinition *
umi_workbench_context_source_registry_find(
    const UmiWorkbenchContextSourceRegistry *registry,
    const char *source_id);
UmiStatus umi_workbench_context_source_registry_remove(
    UmiWorkbenchContextSourceRegistry *registry,
    const char *source_id);
size_t umi_workbench_context_source_registry_count(
    const UmiWorkbenchContextSourceRegistry *registry);

#ifdef __cplusplus
}
#endif
#endif
