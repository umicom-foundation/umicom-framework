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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_CONSOLE_ENTRY_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_CONSOLE_ENTRY_MODEL_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench debug console entry model data shared with callers of this
 * public contract.
 */
typedef struct UmiDebugWorkbenchDebugConsoleEntryModel {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchDebugConsoleEntryModel;

/**
 * Initialise debug workbench debug console entry model from caller-provided values so
 * later operations receive a known state.
 */
UmiStatus umi_debug_workbench_debug_console_entry_model_init(UmiDebugWorkbenchDebugConsoleEntryModel *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
/**
 * Provide the debug workbench debug console entry model set state operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_console_entry_model_set_state(UmiDebugWorkbenchDebugConsoleEntryModel *model, uint32_t state, uint64_t value);
/**
 * Find debug workbench debug console entry model set while leaving the underlying
 * catalogue or model owned by this module.
 */
UmiStatus umi_debug_workbench_debug_console_entry_model_set_selected(UmiDebugWorkbenchDebugConsoleEntryModel *model, bool selected);
/**
 * Provide the debug workbench debug console entry model set enabled operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_console_entry_model_set_enabled(UmiDebugWorkbenchDebugConsoleEntryModel *model, bool enabled);
/**
 * Check that debug workbench debug console entry model satisfies its contract before
 * another service relies on it.
 */
int umi_debug_workbench_debug_console_entry_model_valid(const UmiDebugWorkbenchDebugConsoleEntryModel *model);

#ifdef __cplusplus
}
#endif
#endif
