/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/variable_item.h
 *
 * PURPOSE:
 *   Represent one variable/watch value with lazy-child and change presentation state.
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
#ifndef UMICOM_DEBUG_WORKBENCH_VARIABLE_ITEM_H
#define UMICOM_DEBUG_WORKBENCH_VARIABLE_ITEM_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchVariableItem {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchVariableItem;

UmiStatus umi_debug_workbench_variable_item_init(UmiDebugWorkbenchVariableItem *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
UmiStatus umi_debug_workbench_variable_item_set_state(UmiDebugWorkbenchVariableItem *model, uint32_t state, uint64_t value);
UmiStatus umi_debug_workbench_variable_item_set_selected(UmiDebugWorkbenchVariableItem *model, bool selected);
UmiStatus umi_debug_workbench_variable_item_set_enabled(UmiDebugWorkbenchVariableItem *model, bool enabled);
int umi_debug_workbench_variable_item_valid(const UmiDebugWorkbenchVariableItem *model);

#ifdef __cplusplus
}
#endif
#endif
