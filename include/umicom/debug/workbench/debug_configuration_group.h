/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/debug_configuration_group.h
 *
 * PURPOSE:
 *   Group related debug configurations for presentation and compound launch selection.
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
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_CONFIGURATION_GROUP_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_CONFIGURATION_GROUP_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench debug configuration group data shared with callers of this
 * public contract.
 */
typedef struct UmiDebugWorkbenchDebugConfigurationGroup {
    UmiDebugWorkbenchEntry items[UMI_DEBUG_WORKBENCH_MAX_ITEMS];
    size_t count;
    size_t active_index;
    uint64_t revision;
} UmiDebugWorkbenchDebugConfigurationGroup;

/**
 * Initialise debug workbench debug configuration group from caller-provided values so
 * later operations receive a known state.
 */
void umi_debug_workbench_debug_configuration_group_init(UmiDebugWorkbenchDebugConfigurationGroup *model);
/**
 * Add debug workbench debug configuration group only after its inputs and available
 * capacity have been checked.
 */
UmiStatus umi_debug_workbench_debug_configuration_group_add(UmiDebugWorkbenchDebugConfigurationGroup *model, const UmiDebugWorkbenchEntry *entry);
/**
 * Remove debug workbench debug configuration group while keeping the remaining records in
 * a valid and discoverable state.
 */
UmiStatus umi_debug_workbench_debug_configuration_group_remove(UmiDebugWorkbenchDebugConfigurationGroup *model, const char *id);
/**
 * Provide the debug workbench debug configuration group select operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_configuration_group_select(UmiDebugWorkbenchDebugConfigurationGroup *model, const char *id);
/**
 * Find debug workbench debug configuration group while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiDebugWorkbenchEntry *umi_debug_workbench_debug_configuration_group_find(const UmiDebugWorkbenchDebugConfigurationGroup *model, const char *id);
/**
 * Check that debug workbench debug configuration group satisfies its contract before
 * another service relies on it.
 */
int umi_debug_workbench_debug_configuration_group_valid(const UmiDebugWorkbenchDebugConfigurationGroup *model);

#ifdef __cplusplus
}
#endif
#endif
