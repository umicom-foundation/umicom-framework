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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_SOURCE_REVEAL_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_SOURCE_REVEAL_MODEL_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchDebugSourceRevealModel {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchDebugSourceRevealModel;

UmiStatus umi_debug_workbench_debug_source_reveal_model_init(UmiDebugWorkbenchDebugSourceRevealModel *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
UmiStatus umi_debug_workbench_debug_source_reveal_model_set_state(UmiDebugWorkbenchDebugSourceRevealModel *model, uint32_t state, uint64_t value);
UmiStatus umi_debug_workbench_debug_source_reveal_model_set_selected(UmiDebugWorkbenchDebugSourceRevealModel *model, bool selected);
UmiStatus umi_debug_workbench_debug_source_reveal_model_set_enabled(UmiDebugWorkbenchDebugSourceRevealModel *model, bool enabled);
int umi_debug_workbench_debug_source_reveal_model_valid(const UmiDebugWorkbenchDebugSourceRevealModel *model);

#ifdef __cplusplus
}
#endif
#endif
