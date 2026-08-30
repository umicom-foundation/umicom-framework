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

typedef struct UmiWorkbenchSelectionProviderRegistry {
    UmiWorkbenchSelectionProviderDescriptor *items;
    size_t count;
    size_t capacity;
    uint64_t revision;
} UmiWorkbenchSelectionProviderRegistry;

void umi_workbench_selection_provider_registry_init(
    UmiWorkbenchSelectionProviderRegistry *registry);
void umi_workbench_selection_provider_registry_destroy(
    UmiWorkbenchSelectionProviderRegistry *registry);
UmiStatus umi_workbench_selection_provider_registry_upsert(
    UmiWorkbenchSelectionProviderRegistry *registry,
    const UmiWorkbenchSelectionProviderDescriptor *descriptor);
const UmiWorkbenchSelectionProviderDescriptor *
umi_workbench_selection_provider_registry_find(
    const UmiWorkbenchSelectionProviderRegistry *registry,
    const char *provider_id);
const UmiWorkbenchSelectionProviderDescriptor *
umi_workbench_selection_provider_registry_find_panel(
    const UmiWorkbenchSelectionProviderRegistry *registry,
    const char *panel_id);
UmiStatus umi_workbench_selection_provider_registry_remove(
    UmiWorkbenchSelectionProviderRegistry *registry,
    const char *provider_id);

#ifdef __cplusplus
}
#endif
#endif
