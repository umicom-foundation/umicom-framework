/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/binding_registry.h
 *
 * PURPOSE:
 *   Manage panel bindings with bounded dynamically allocated storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_BINDING_REGISTRY_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_BINDING_REGISTRY_H

#include "umicom/workbench_context_link/panel_binding.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link binding registry data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextLinkBindingRegistry {
    UmiWorkbenchContextLinkPanelBinding *items;
    size_t count;
    size_t capacity;
    uint64_t revision;
} UmiWorkbenchContextLinkBindingRegistry;

/**
 * Initialise workbench context link binding registry from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_binding_registry_init(
    UmiWorkbenchContextLinkBindingRegistry *registry);
/**
 * Release or reset state held by workbench context link binding registry so the same
 * storage can be reused safely.
 */
void umi_workbench_context_link_binding_registry_destroy(
    UmiWorkbenchContextLinkBindingRegistry *registry);
/**
 * Provide the workbench context link binding registry upsert operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_link_binding_registry_upsert(
    UmiWorkbenchContextLinkBindingRegistry *registry,
    const UmiWorkbenchContextLinkPanelBinding *binding);
/**
 * Remove workbench context link binding registry while keeping the remaining records in a
 * valid and discoverable state.
 */
UmiStatus umi_workbench_context_link_binding_registry_remove(
    UmiWorkbenchContextLinkBindingRegistry *registry,
    const char *binding_id);
/**
 * Provide the workbench context link binding registry remove panel operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_binding_registry_remove_panel(
    UmiWorkbenchContextLinkBindingRegistry *registry,
    const char *panel_id);
/**
 * Find workbench context link binding registry while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiWorkbenchContextLinkPanelBinding *
umi_workbench_context_link_binding_registry_find(
    const UmiWorkbenchContextLinkBindingRegistry *registry,
    const char *binding_id);
/**
 * Return the number of records represented by workbench context link binding registry
 * without changing their state.
 */
size_t umi_workbench_context_link_binding_registry_count(
    const UmiWorkbenchContextLinkBindingRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
