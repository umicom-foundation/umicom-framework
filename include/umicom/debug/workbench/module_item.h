/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/module_item.h
 *
 * PURPOSE:
 *   Represent one loaded module/image with address and symbol loading status.
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
#ifndef UMICOM_DEBUG_WORKBENCH_MODULE_ITEM_H
#define UMICOM_DEBUG_WORKBENCH_MODULE_ITEM_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchModuleItem {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchModuleItem;

UmiStatus umi_debug_workbench_module_item_init(UmiDebugWorkbenchModuleItem *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
UmiStatus umi_debug_workbench_module_item_set_state(UmiDebugWorkbenchModuleItem *model, uint32_t state, uint64_t value);
UmiStatus umi_debug_workbench_module_item_set_selected(UmiDebugWorkbenchModuleItem *model, bool selected);
UmiStatus umi_debug_workbench_module_item_set_enabled(UmiDebugWorkbenchModuleItem *model, bool enabled);
int umi_debug_workbench_module_item_valid(const UmiDebugWorkbenchModuleItem *model);

#ifdef __cplusplus
}
#endif
#endif
