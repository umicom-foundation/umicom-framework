/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/register_value_model.h
 *
 * PURPOSE:
 *   Represent one register value with changed-value highlighting metadata.
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
#ifndef UMICOM_DEBUG_WORKBENCH_REGISTER_VALUE_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_REGISTER_VALUE_MODEL_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench register value model data shared with callers of this
 * public contract.
 */
typedef struct UmiDebugWorkbenchRegisterValueModel {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchRegisterValueModel;

/**
 * Initialise debug workbench register value model from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_debug_workbench_register_value_model_init(UmiDebugWorkbenchRegisterValueModel *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
/**
 * Provide the debug workbench register value model set state operation used by this module
 * and its client applications.
 */
UmiStatus umi_debug_workbench_register_value_model_set_state(UmiDebugWorkbenchRegisterValueModel *model, uint32_t state, uint64_t value);
/**
 * Find debug workbench register value model set while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_debug_workbench_register_value_model_set_selected(UmiDebugWorkbenchRegisterValueModel *model, bool selected);
/**
 * Provide the debug workbench register value model set enabled operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_register_value_model_set_enabled(UmiDebugWorkbenchRegisterValueModel *model, bool enabled);
/**
 * Check that debug workbench register value model satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_register_value_model_valid(const UmiDebugWorkbenchRegisterValueModel *model);

#ifdef __cplusplus
}
#endif
#endif
