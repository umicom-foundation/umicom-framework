/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/debug_navigation_checkpoint.h
 *
 * PURPOSE:
 *   Represent a source/instruction navigation checkpoint tied to a debug session.
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
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_NAVIGATION_CHECKPOINT_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_NAVIGATION_CHECKPOINT_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchDebugNavigationCheckpoint {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchDebugNavigationCheckpoint;

UmiStatus umi_debug_workbench_debug_navigation_checkpoint_init(UmiDebugWorkbenchDebugNavigationCheckpoint *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
UmiStatus umi_debug_workbench_debug_navigation_checkpoint_set_state(UmiDebugWorkbenchDebugNavigationCheckpoint *model, uint32_t state, uint64_t value);
UmiStatus umi_debug_workbench_debug_navigation_checkpoint_set_selected(UmiDebugWorkbenchDebugNavigationCheckpoint *model, bool selected);
UmiStatus umi_debug_workbench_debug_navigation_checkpoint_set_enabled(UmiDebugWorkbenchDebugNavigationCheckpoint *model, bool enabled);
int umi_debug_workbench_debug_navigation_checkpoint_valid(const UmiDebugWorkbenchDebugNavigationCheckpoint *model);

#ifdef __cplusplus
}
#endif
#endif
