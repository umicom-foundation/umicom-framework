/* Umicom Framework breakpoint synchronization plan. Sammy Hegab, Umicom Foundation, MIT. */
#ifndef UMICOM_DEBUG_BREAKPOINT_PLAN_H
#define UMICOM_DEBUG_BREAKPOINT_PLAN_H
#include "umicom/debug/breakpoint.h"
#include "umicom/protocol/dap_extended.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_DEBUG_BREAKPOINT_PLAN_CAPACITY 256U
typedef struct UmiDebugBreakpointPlan{char source_path[1024];UmiDapSourceBreakpoint items[UMI_DEBUG_BREAKPOINT_PLAN_CAPACITY];char conditions[UMI_DEBUG_BREAKPOINT_PLAN_CAPACITY][512];char hit_conditions[UMI_DEBUG_BREAKPOINT_PLAN_CAPACITY][128];char log_messages[UMI_DEBUG_BREAKPOINT_PLAN_CAPACITY][512];size_t count;uint64_t revision;}UmiDebugBreakpointPlan;
UmiStatus umi_debug_breakpoint_plan_build(const UmiDebugBreakpointRegistry *registry,const char *source_path,UmiDebugBreakpointPlan *out_plan);
#ifdef __cplusplus
}
#endif
#endif
