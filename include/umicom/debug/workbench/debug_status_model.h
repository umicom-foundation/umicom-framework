/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/debug_status_model.h
 *
 * PURPOSE:
 *   Aggregate active session, stop reason and inspection-count status for workbench chrome.
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
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_STATUS_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_STATUS_MODEL_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchDebugStatusModel {
    char session_id[UMI_DEBUG_WORKBENCH_ID_CAPACITY];
    char stop_reason[UMI_DEBUG_WORKBENCH_TEXT_CAPACITY];
    UmiDebugWorkbenchSessionPhase phase;
    uint32_t thread_count;
    uint32_t frame_count;
    uint32_t variable_count;
    uint64_t revision;
} UmiDebugWorkbenchDebugStatusModel;

UmiStatus umi_debug_workbench_debug_status_model_init(UmiDebugWorkbenchDebugStatusModel *model, const char *session_id);
UmiStatus umi_debug_workbench_debug_status_model_update(UmiDebugWorkbenchDebugStatusModel *model, UmiDebugWorkbenchSessionPhase phase, const char *stop_reason, uint32_t threads, uint32_t frames, uint32_t variables);
int umi_debug_workbench_debug_status_model_valid(const UmiDebugWorkbenchDebugStatusModel *model);

#ifdef __cplusplus
}
#endif
#endif
