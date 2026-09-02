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

/**
 * Represent the workbench context source registry data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextSourceRegistry {
    UmiWorkbenchContextSourceDefinition *items;
    size_t count;
    size_t capacity;
    uint64_t revision;
} UmiWorkbenchContextSourceRegistry;

/**
 * Initialise workbench context source registry from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_source_registry_init(
    UmiWorkbenchContextSourceRegistry *registry);
/**
 * Release or reset state held by workbench context source registry so the same storage can
 * be reused safely.
 */
void umi_workbench_context_source_registry_destroy(
    UmiWorkbenchContextSourceRegistry *registry);
/**
 * Provide the workbench context source registry upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_source_registry_upsert(
    UmiWorkbenchContextSourceRegistry *registry,
    const UmiWorkbenchContextSourceDefinition *definition);
/**
 * Find workbench context source registry while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiWorkbenchContextSourceDefinition *
umi_workbench_context_source_registry_find(
    const UmiWorkbenchContextSourceRegistry *registry,
    const char *source_id);
/**
 * Remove workbench context source registry while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_workbench_context_source_registry_remove(
    UmiWorkbenchContextSourceRegistry *registry,
    const char *source_id);
/**
 * Return the number of records represented by workbench context source registry without
 * changing their state.
 */
size_t umi_workbench_context_source_registry_count(
    const UmiWorkbenchContextSourceRegistry *registry);

#ifdef __cplusplus
}
#endif
#endif
