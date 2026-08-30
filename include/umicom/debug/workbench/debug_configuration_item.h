/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/debug_configuration_item.h
 *
 * PURPOSE:
 *   Represent one launch or attach configuration surfaced by the workbench.
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
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_CONFIGURATION_ITEM_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_CONFIGURATION_ITEM_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchDebugConfigurationItem {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchDebugConfigurationItem;

UmiStatus umi_debug_workbench_debug_configuration_item_init(UmiDebugWorkbenchDebugConfigurationItem *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
UmiStatus umi_debug_workbench_debug_configuration_item_set_state(UmiDebugWorkbenchDebugConfigurationItem *model, uint32_t state, uint64_t value);
UmiStatus umi_debug_workbench_debug_configuration_item_set_selected(UmiDebugWorkbenchDebugConfigurationItem *model, bool selected);
UmiStatus umi_debug_workbench_debug_configuration_item_set_enabled(UmiDebugWorkbenchDebugConfigurationItem *model, bool enabled);
int umi_debug_workbench_debug_configuration_item_valid(const UmiDebugWorkbenchDebugConfigurationItem *model);

#ifdef __cplusplus
}
#endif
#endif
