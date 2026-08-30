/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/data_breakpoint.h
 *
 * PURPOSE:
 *   Define bounded data-breakpoint inputs for DAP setDataBreakpoints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_DATA_BREAKPOINT_H
#define UMICOM_DEBUG_RUNTIME_DATA_BREAKPOINT_H
#include "umicom/debug_runtime/types.h"
typedef struct UmiDebugRuntimeDataBreakpoint {
    char data_id[512];
    char access_type[64];
    char condition[512];
    char hit_condition[128];
} UmiDebugRuntimeDataBreakpoint;
#endif
