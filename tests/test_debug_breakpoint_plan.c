/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_debug_breakpoint_plan.c
 *
 * PURPOSE:
 *   Implement the test debug breakpoint plan behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework breakpoint plan test. Sammy Hegab, Umicom Foundation, MIT. */
#include <assert.h>
#include <string.h>
#include "umicom/debug/breakpoint_plan.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiDebugBreakpointRegistry*r=NULL;UmiDebugBreakpointSnapshot b={0};UmiDebugBreakpointPlan p;strcpy(b.id,"main:12");strcpy(b.uri,"main.c");strcpy(b.condition,"x>0");b.line=12U;b.enabled=1;assert(umi_debug_breakpoint_registry_create(&r)==UMI_STATUS_OK);assert(umi_debug_breakpoint_registry_upsert(r,&b)==UMI_STATUS_OK);assert(umi_debug_breakpoint_plan_build(r,"main.c",&p)==UMI_STATUS_OK);assert(p.count==1U&&p.items[0].line==12);umi_debug_breakpoint_registry_destroy(r);return 0;}
