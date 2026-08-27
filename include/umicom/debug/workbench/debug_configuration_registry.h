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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_CONFIGURATION_REGISTRY_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_CONFIGURATION_REGISTRY_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchDebugConfigurationRegistry {
    UmiDebugWorkbenchEntry items[UMI_DEBUG_WORKBENCH_MAX_ITEMS];
    size_t count;
    size_t active_index;
    uint64_t revision;
} UmiDebugWorkbenchDebugConfigurationRegistry;

void umi_debug_workbench_debug_configuration_registry_init(UmiDebugWorkbenchDebugConfigurationRegistry *model);
UmiStatus umi_debug_workbench_debug_configuration_registry_add(UmiDebugWorkbenchDebugConfigurationRegistry *model, const UmiDebugWorkbenchEntry *entry);
UmiStatus umi_debug_workbench_debug_configuration_registry_remove(UmiDebugWorkbenchDebugConfigurationRegistry *model, const char *id);
UmiStatus umi_debug_workbench_debug_configuration_registry_select(UmiDebugWorkbenchDebugConfigurationRegistry *model, const char *id);
const UmiDebugWorkbenchEntry *umi_debug_workbench_debug_configuration_registry_find(const UmiDebugWorkbenchDebugConfigurationRegistry *model, const char *id);
int umi_debug_workbench_debug_configuration_registry_valid(const UmiDebugWorkbenchDebugConfigurationRegistry *model);

#ifdef __cplusplus
}
#endif
#endif
