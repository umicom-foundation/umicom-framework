/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/debug_source_reveal_model.h
 *
 * PURPOSE:
 *   Represent source reveal, line focus and instruction-pointer presentation intent.
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
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_SOURCE_REVEAL_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_SOURCE_REVEAL_MODEL_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench debug source reveal model data shared with callers of this
 * public contract.
 */
typedef struct UmiDebugWorkbenchDebugSourceRevealModel {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchDebugSourceRevealModel;

/**
 * Initialise debug workbench debug source reveal model from caller-provided values so
 * later operations receive a known state.
 */
UmiStatus umi_debug_workbench_debug_source_reveal_model_init(UmiDebugWorkbenchDebugSourceRevealModel *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
/**
 * Provide the debug workbench debug source reveal model set state operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_source_reveal_model_set_state(UmiDebugWorkbenchDebugSourceRevealModel *model, uint32_t state, uint64_t value);
/**
 * Find debug workbench debug source reveal model set while leaving the underlying
 * catalogue or model owned by this module.
 */
UmiStatus umi_debug_workbench_debug_source_reveal_model_set_selected(UmiDebugWorkbenchDebugSourceRevealModel *model, bool selected);
/**
 * Provide the debug workbench debug source reveal model set enabled operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_source_reveal_model_set_enabled(UmiDebugWorkbenchDebugSourceRevealModel *model, bool enabled);
/**
 * Check that debug workbench debug source reveal model satisfies its contract before
 * another service relies on it.
 */
int umi_debug_workbench_debug_source_reveal_model_valid(const UmiDebugWorkbenchDebugSourceRevealModel *model);

#ifdef __cplusplus
}
#endif
#endif
