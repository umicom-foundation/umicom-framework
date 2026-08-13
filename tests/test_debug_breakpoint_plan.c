/* Umicom Framework breakpoint plan test. Sammy Hegab, Umicom Foundation, MIT. */
#include <assert.h>
#include <string.h>
#include "umicom/debug/breakpoint_plan.h"
int main(void){UmiDebugBreakpointRegistry*r=NULL;UmiDebugBreakpointSnapshot b={0};UmiDebugBreakpointPlan p;strcpy(b.id,"main:12");strcpy(b.uri,"main.c");strcpy(b.condition,"x>0");b.line=12U;b.enabled=1;assert(umi_debug_breakpoint_registry_create(&r)==UMI_STATUS_OK);assert(umi_debug_breakpoint_registry_upsert(r,&b)==UMI_STATUS_OK);assert(umi_debug_breakpoint_plan_build(r,"main.c",&p)==UMI_STATUS_OK);assert(p.count==1U&&p.items[0].line==12);umi_debug_breakpoint_registry_destroy(r);return 0;}
