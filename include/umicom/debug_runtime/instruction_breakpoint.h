/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/instruction_breakpoint.h
 *
 * PURPOSE:
 *   Define bounded instruction-breakpoint inputs for DAP.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_INSTRUCTION_BREAKPOINT_H
#define UMICOM_DEBUG_RUNTIME_INSTRUCTION_BREAKPOINT_H
#include "umicom/debug_runtime/types.h"
typedef struct UmiDebugRuntimeInstructionBreakpoint {
    char instruction_reference[128];
    int64_t offset;
    char condition[512];
    char hit_condition[128];
} UmiDebugRuntimeInstructionBreakpoint;
#endif
