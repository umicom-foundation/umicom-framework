/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/debug_configuration_registry.h
 *
 * PURPOSE:
 *   Maintain deterministic launch and attach configuration ordering and selection.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical Debug Service/DAP
 *   runtime state; Studio remains a thin frontend and owns no reusable debug
 *   semantics, adapter protocol, breakpoint engine or inspection engine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_CONFIGURATION_REGISTRY_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_CONFIGURATION_REGISTRY_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench debug configuration registry data shared with callers of
 * this public contract.
 */
typedef struct UmiDebugWorkbenchDebugConfigurationRegistry {
    UmiDebugWorkbenchEntry items[UMI_DEBUG_WORKBENCH_MAX_ITEMS];
    size_t count;
    size_t active_index;
    uint64_t revision;
} UmiDebugWorkbenchDebugConfigurationRegistry;

/**
 * Initialise debug workbench debug configuration registry from caller-provided values so
 * later operations receive a known state.
 */
void umi_debug_workbench_debug_configuration_registry_init(UmiDebugWorkbenchDebugConfigurationRegistry *model);
/**
 * Add debug workbench debug configuration registry only after its inputs and available
 * capacity have been checked.
 */
UmiStatus umi_debug_workbench_debug_configuration_registry_add(UmiDebugWorkbenchDebugConfigurationRegistry *model, const UmiDebugWorkbenchEntry *entry);
/**
 * Remove debug workbench debug configuration registry while keeping the remaining records
 * in a valid and discoverable state.
 */
UmiStatus umi_debug_workbench_debug_configuration_registry_remove(UmiDebugWorkbenchDebugConfigurationRegistry *model, const char *id);
/**
 * Provide the debug workbench debug configuration registry select operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_configuration_registry_select(UmiDebugWorkbenchDebugConfigurationRegistry *model, const char *id);
/**
 * Find debug workbench debug configuration registry while leaving the underlying catalogue
 * or model owned by this module.
 */
const UmiDebugWorkbenchEntry *umi_debug_workbench_debug_configuration_registry_find(const UmiDebugWorkbenchDebugConfigurationRegistry *model, const char *id);
/**
 * Check that debug workbench debug configuration registry satisfies its contract before
 * another service relies on it.
 */
int umi_debug_workbench_debug_configuration_registry_valid(const UmiDebugWorkbenchDebugConfigurationRegistry *model);

#ifdef __cplusplus
}
#endif
#endif
