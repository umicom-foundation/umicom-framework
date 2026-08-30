/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/debug_attach_request.h
 *
 * PURPOSE:
 *   Capture a validated process-attach request before dispatch to the canonical debug runtime.
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
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_ATTACH_REQUEST_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_ATTACH_REQUEST_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchDebugAttachRequest {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchDebugAttachRequest;

UmiStatus umi_debug_workbench_debug_attach_request_init(UmiDebugWorkbenchDebugAttachRequest *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
UmiStatus umi_debug_workbench_debug_attach_request_set_state(UmiDebugWorkbenchDebugAttachRequest *model, uint32_t state, uint64_t value);
UmiStatus umi_debug_workbench_debug_attach_request_set_selected(UmiDebugWorkbenchDebugAttachRequest *model, bool selected);
UmiStatus umi_debug_workbench_debug_attach_request_set_enabled(UmiDebugWorkbenchDebugAttachRequest *model, bool enabled);
int umi_debug_workbench_debug_attach_request_valid(const UmiDebugWorkbenchDebugAttachRequest *model);

#ifdef __cplusplus
}
#endif
#endif
