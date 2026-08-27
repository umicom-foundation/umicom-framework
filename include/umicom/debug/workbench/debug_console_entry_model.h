/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/debug_console_entry_model.h
 *
 * PURPOSE:
 *   Represent one debug-console input, output, telemetry or diagnostic entry.
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
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_CONSOLE_ENTRY_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_CONSOLE_ENTRY_MODEL_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchDebugConsoleEntryModel {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchDebugConsoleEntryModel;

UmiStatus umi_debug_workbench_debug_console_entry_model_init(UmiDebugWorkbenchDebugConsoleEntryModel *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
UmiStatus umi_debug_workbench_debug_console_entry_model_set_state(UmiDebugWorkbenchDebugConsoleEntryModel *model, uint32_t state, uint64_t value);
UmiStatus umi_debug_workbench_debug_console_entry_model_set_selected(UmiDebugWorkbenchDebugConsoleEntryModel *model, bool selected);
UmiStatus umi_debug_workbench_debug_console_entry_model_set_enabled(UmiDebugWorkbenchDebugConsoleEntryModel *model, bool enabled);
int umi_debug_workbench_debug_console_entry_model_valid(const UmiDebugWorkbenchDebugConsoleEntryModel *model);

#ifdef __cplusplus
}
#endif
#endif
