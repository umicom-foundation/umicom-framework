/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/debug_conformance.h
 *
 * PURPOSE:
 *   Evaluate required debugger-workbench capabilities for frontend conformance.
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
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_CONFORMANCE_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_CONFORMANCE_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEBUG_WORKBENCH_CONFORMANCE_SESSIONS UINT64_C(1)
#define UMI_DEBUG_WORKBENCH_CONFORMANCE_BREAKPOINTS UINT64_C(2)
#define UMI_DEBUG_WORKBENCH_CONFORMANCE_INSPECTION UINT64_C(4)
#define UMI_DEBUG_WORKBENCH_CONFORMANCE_CONSOLE UINT64_C(8)
#define UMI_DEBUG_WORKBENCH_CONFORMANCE_MEMORY UINT64_C(16)
#define UMI_DEBUG_WORKBENCH_CONFORMANCE_DISASSEMBLY UINT64_C(32)

typedef struct UmiDebugWorkbenchDebugConformance { uint64_t required; uint64_t available; uint64_t revision; } UmiDebugWorkbenchDebugConformance;

void umi_debug_workbench_debug_conformance_init(UmiDebugWorkbenchDebugConformance *model, uint64_t required);
UmiStatus umi_debug_workbench_debug_conformance_set_available(UmiDebugWorkbenchDebugConformance *model, uint64_t capability, bool available);
uint64_t umi_debug_workbench_debug_conformance_missing(const UmiDebugWorkbenchDebugConformance *model);
int umi_debug_workbench_debug_conformance_passes(const UmiDebugWorkbenchDebugConformance *model);

#ifdef __cplusplus
}
#endif
#endif
