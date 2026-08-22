/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/binding_registry.h
 *
 * PURPOSE:
 *   Manage panel bindings with bounded dynamically allocated storage.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_BINDING_REGISTRY_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_BINDING_REGISTRY_H

#include "umicom/workbench_context_link/panel_binding.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkBindingRegistry {
    UmiWorkbenchContextLinkPanelBinding *items;
    size_t count;
    size_t capacity;
    uint64_t revision;
} UmiWorkbenchContextLinkBindingRegistry;

void umi_workbench_context_link_binding_registry_init(
    UmiWorkbenchContextLinkBindingRegistry *registry);
void umi_workbench_context_link_binding_registry_destroy(
    UmiWorkbenchContextLinkBindingRegistry *registry);
UmiStatus umi_workbench_context_link_binding_registry_upsert(
    UmiWorkbenchContextLinkBindingRegistry *registry,
    const UmiWorkbenchContextLinkPanelBinding *binding);
UmiStatus umi_workbench_context_link_binding_registry_remove(
    UmiWorkbenchContextLinkBindingRegistry *registry,
    const char *binding_id);
UmiStatus umi_workbench_context_link_binding_registry_remove_panel(
    UmiWorkbenchContextLinkBindingRegistry *registry,
    const char *panel_id);
const UmiWorkbenchContextLinkPanelBinding *
umi_workbench_context_link_binding_registry_find(
    const UmiWorkbenchContextLinkBindingRegistry *registry,
    const char *binding_id);
size_t umi_workbench_context_link_binding_registry_count(
    const UmiWorkbenchContextLinkBindingRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
