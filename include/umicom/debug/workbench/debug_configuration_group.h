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

typedef struct UmiDebugWorkbenchDebugConfigurationGroup {
    UmiDebugWorkbenchEntry items[UMI_DEBUG_WORKBENCH_MAX_ITEMS];
    size_t count;
    size_t active_index;
    uint64_t revision;
} UmiDebugWorkbenchDebugConfigurationGroup;

void umi_debug_workbench_debug_configuration_group_init(UmiDebugWorkbenchDebugConfigurationGroup *model);
UmiStatus umi_debug_workbench_debug_configuration_group_add(UmiDebugWorkbenchDebugConfigurationGroup *model, const UmiDebugWorkbenchEntry *entry);
UmiStatus umi_debug_workbench_debug_configuration_group_remove(UmiDebugWorkbenchDebugConfigurationGroup *model, const char *id);
UmiStatus umi_debug_workbench_debug_configuration_group_select(UmiDebugWorkbenchDebugConfigurationGroup *model, const char *id);
const UmiDebugWorkbenchEntry *umi_debug_workbench_debug_configuration_group_find(const UmiDebugWorkbenchDebugConfigurationGroup *model, const char *id);
int umi_debug_workbench_debug_configuration_group_valid(const UmiDebugWorkbenchDebugConfigurationGroup *model);

#ifdef __cplusplus
}
#endif
#endif
