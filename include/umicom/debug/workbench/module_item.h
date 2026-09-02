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

/**
 * Represent the debug workbench module item data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugWorkbenchModuleItem {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchModuleItem;

/**
 * Initialise debug workbench module item from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_debug_workbench_module_item_init(UmiDebugWorkbenchModuleItem *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
/**
 * Provide the debug workbench module item set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_workbench_module_item_set_state(UmiDebugWorkbenchModuleItem *model, uint32_t state, uint64_t value);
/**
 * Find debug workbench module item set while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_debug_workbench_module_item_set_selected(UmiDebugWorkbenchModuleItem *model, bool selected);
/**
 * Provide the debug workbench module item set enabled operation used by this module and
 * its client applications.
 */
UmiStatus umi_debug_workbench_module_item_set_enabled(UmiDebugWorkbenchModuleItem *model, bool enabled);
/**
 * Check that debug workbench module item satisfies its contract before another service
 * relies on it.
 */
int umi_debug_workbench_module_item_valid(const UmiDebugWorkbenchModuleItem *model);

#ifdef __cplusplus
}
#endif
#endif
