/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/register_group_model.h
 *
 * PURPOSE:
 *   Represent one architecture register group and its display state.
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
#ifndef UMICOM_DEBUG_WORKBENCH_REGISTER_GROUP_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_REGISTER_GROUP_MODEL_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench register group model data shared with callers of this
 * public contract.
 */
typedef struct UmiDebugWorkbenchRegisterGroupModel {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchRegisterGroupModel;

/**
 * Initialise debug workbench register group model from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_debug_workbench_register_group_model_init(UmiDebugWorkbenchRegisterGroupModel *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
/**
 * Provide the debug workbench register group model set state operation used by this module
 * and its client applications.
 */
UmiStatus umi_debug_workbench_register_group_model_set_state(UmiDebugWorkbenchRegisterGroupModel *model, uint32_t state, uint64_t value);
/**
 * Find debug workbench register group model set while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_debug_workbench_register_group_model_set_selected(UmiDebugWorkbenchRegisterGroupModel *model, bool selected);
/**
 * Provide the debug workbench register group model set enabled operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_register_group_model_set_enabled(UmiDebugWorkbenchRegisterGroupModel *model, bool enabled);
/**
 * Check that debug workbench register group model satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_register_group_model_valid(const UmiDebugWorkbenchRegisterGroupModel *model);

#ifdef __cplusplus
}
#endif
#endif
