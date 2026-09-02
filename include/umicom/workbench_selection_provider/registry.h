/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/registry.h
 *
 * PURPOSE:
 *   Maintain a bounded dynamic catalogue of structured-selection providers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_REGISTRY_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_REGISTRY_H
#include "umicom/workbench_selection_provider/descriptor.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider registry data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchSelectionProviderRegistry {
    UmiWorkbenchSelectionProviderDescriptor *items;
    size_t count;
    size_t capacity;
    uint64_t revision;
} UmiWorkbenchSelectionProviderRegistry;

/**
 * Initialise workbench selection provider registry from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_provider_registry_init(
    UmiWorkbenchSelectionProviderRegistry *registry);
/**
 * Release or reset state held by workbench selection provider registry so the same storage
 * can be reused safely.
 */
void umi_workbench_selection_provider_registry_destroy(
    UmiWorkbenchSelectionProviderRegistry *registry);
/**
 * Provide the workbench selection provider registry upsert operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_provider_registry_upsert(
    UmiWorkbenchSelectionProviderRegistry *registry,
    const UmiWorkbenchSelectionProviderDescriptor *descriptor);
/**
 * Find workbench selection provider registry while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiWorkbenchSelectionProviderDescriptor *
umi_workbench_selection_provider_registry_find(
    const UmiWorkbenchSelectionProviderRegistry *registry,
    const char *provider_id);
/**
 * Provide the workbench selection provider registry find panel operation used by this
 * module and its client applications.
 */
const UmiWorkbenchSelectionProviderDescriptor *
umi_workbench_selection_provider_registry_find_panel(
    const UmiWorkbenchSelectionProviderRegistry *registry,
    const char *panel_id);
/**
 * Remove workbench selection provider registry while keeping the remaining records in a
 * valid and discoverable state.
 */
UmiStatus umi_workbench_selection_provider_registry_remove(
    UmiWorkbenchSelectionProviderRegistry *registry,
    const char *provider_id);

#ifdef __cplusplus
}
#endif
#endif
