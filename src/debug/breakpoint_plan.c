/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/breakpoint_plan.c
 *
 * PURPOSE:
 *   Implement the breakpoint plan behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework breakpoint synchronization plan. Sammy Hegab, Umicom Foundation, MIT. */
#include "umicom/debug/breakpoint_plan.h"
#include <stdio.h>
#include <string.h>
/*
 * Provide the debug breakpoint plan build operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_breakpoint_plan_build(const UmiDebugBreakpointRegistry*r,const char*path,UmiDebugBreakpointPlan*out){size_t i,n;int written;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||path==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(out,0,sizeof(*out));written=snprintf(out->source_path,sizeof(out->source_path),"%s",path);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(written<0||(size_t)written>=sizeof(out->source_path))return UMI_STATUS_CAPACITY_EXCEEDED;n=umi_debug_breakpoint_registry_count(r);/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<n;++i){UmiDebugBreakpointSnapshot b;size_t p;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_debug_breakpoint_registry_at(r,i,&b)!=UMI_STATUS_OK||!b.enabled||strcmp(b.uri,path)!=0)continue;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out->count>=UMI_DEBUG_BREAKPOINT_PLAN_CAPACITY)return UMI_STATUS_CAPACITY_EXCEEDED;p=out->count++;(void)snprintf(out->conditions[p],sizeof(out->conditions[p]),"%s",b.condition);(void)snprintf(out->log_messages[p],sizeof(out->log_messages[p]),"%s",b.log_message);out->items[p].line=(int)b.line;out->items[p].column=(int)b.column;out->items[p].condition=out->conditions[p];out->items[p].hit_condition=out->hit_conditions[p];out->items[p].log_message=out->log_messages[p];/* Protect caller-owned memory by checking that required state is available before it is used. */ if(b.revision>out->revision)out->revision=b.revision;}return UMI_STATUS_OK;}
