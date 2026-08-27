/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/call_stack_frame_item.h
 *
 * PURPOSE:
 *   Represent one call-stack frame and source-location reveal state.
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
#ifndef UMICOM_DEBUG_WORKBENCH_CALL_STACK_FRAME_ITEM_H
#define UMICOM_DEBUG_WORKBENCH_CALL_STACK_FRAME_ITEM_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchCallStackFrameItem {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchCallStackFrameItem;

UmiStatus umi_debug_workbench_call_stack_frame_item_init(UmiDebugWorkbenchCallStackFrameItem *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
UmiStatus umi_debug_workbench_call_stack_frame_item_set_state(UmiDebugWorkbenchCallStackFrameItem *model, uint32_t state, uint64_t value);
UmiStatus umi_debug_workbench_call_stack_frame_item_set_selected(UmiDebugWorkbenchCallStackFrameItem *model, bool selected);
UmiStatus umi_debug_workbench_call_stack_frame_item_set_enabled(UmiDebugWorkbenchCallStackFrameItem *model, bool enabled);
int umi_debug_workbench_call_stack_frame_item_valid(const UmiDebugWorkbenchCallStackFrameItem *model);

#ifdef __cplusplus
}
#endif
#endif
