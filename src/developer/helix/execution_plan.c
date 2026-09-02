/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/helix/execution_plan.c
 *
 * PURPOSE:
 *   Order executable steps using dependency edges and reject cyclic or unapproved plans.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/execution_plan.h"
/* Copy validated step evidence into the bounded plan. */
UmiStatus umi_helix_execution_plan_add(UmiHelixExecutionPlan *p,const UmiHelixExecutionStep *s){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||s==NULL||umi_helix_execution_step_validate(s)!=UMI_STATUS_OK){ return UMI_STATUS_INVALID_ARGUMENT; } /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->count>=UMI_HELIX_MAX_ITEMS){ return UMI_STATUS_CAPACITY_EXCEEDED; } p->steps[p->count++]=*s; return UMI_STATUS_OK; }
/* Register one valid dependency edge. */
UmiStatus umi_helix_execution_plan_depend(UmiHelixExecutionPlan *p,size_t a,size_t b){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||a>=p->count||b>=p->count||a==b){ return UMI_STATUS_INVALID_ARGUMENT; } /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->edge_count>=UMI_HELIX_MAX_EDGES){ return UMI_STATUS_CAPACITY_EXCEEDED; } p->edges[p->edge_count++]=(UmiHelixExecutionEdge){(uint16_t)a,(uint16_t)b}; return UMI_STATUS_OK; }
/* Topologically order approved execution steps and reject cyclic plans. */
UmiStatus umi_helix_execution_plan_order(const UmiHelixExecutionPlan *p,size_t *out,size_t cap,size_t *n){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||out==NULL||n==NULL||cap<p->count){ return UMI_STATUS_INVALID_ARGUMENT; } /* Protect caller-owned memory by checking that required state is available before it is used. */ if(!p->approved){ return UMI_STATUS_PERMISSION_DENIED; } size_t indeg[UMI_HELIX_MAX_ITEMS]={0}; bool used[UMI_HELIX_MAX_ITEMS]={false}; /* Visit each bounded item once so every record receives the same rule. */ for(size_t e=0;e<p->edge_count;e++) indeg[p->edges[e].to]++; *n=0U; /* Continue only while work remains available; the loop body advances the state on each pass. */ while(*n<p->count){ size_t pick=p->count; /* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0;i<p->count;i++) /* Protect caller-owned memory by checking that required state is available before it is used. */ if(!used[i]&&indeg[i]==0U){pick=i;break;} /* Protect caller-owned memory by checking that required state is available before it is used. */ if(pick==p->count){ return UMI_STATUS_INVALID_STATE; } used[pick]=true; out[(*n)++]=pick; /* Visit each bounded item once so every record receives the same rule. */ for(size_t e=0;e<p->edge_count;e++) /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->edges[e].from==pick&&indeg[p->edges[e].to]>0U) indeg[p->edges[e].to]--; } return UMI_STATUS_OK; }
