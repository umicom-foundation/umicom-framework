/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/function_breakpoint.h
 *
 * PURPOSE:
 *   Define bounded DAP function-breakpoint input before it is verified by an
 *   adapter. Existing source/advanced breakpoint registries remain unchanged.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_FUNCTION_BREAKPOINT_H
#define UMICOM_DEBUG_RUNTIME_FUNCTION_BREAKPOINT_H
#include "umicom/debug_runtime/types.h"
typedef struct UmiDebugRuntimeFunctionBreakpoint {
    char name[512];
    char condition[512];
    char hit_condition[128];
} UmiDebugRuntimeFunctionBreakpoint;
#endif
