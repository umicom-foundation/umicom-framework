/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/helix/plan_graph.c
 *
 * PURPOSE:
 *   Store a bounded directed task graph and provide dependency/cycle/topological-order validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/plan_graph.h"
#include <string.h>
/* Clear all nodes and edges. */
void umi_helix_plan_graph_init(UmiHelixPlanGraph *g){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(g!=NULL) memset(g,0,sizeof(*g)); }
/* Add one unique bounded identifier. */
UmiStatus umi_helix_plan_graph_add(UmiHelixPlanGraph *g,const char *id){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(g==NULL||!umi_helix_valid_id(id)){ return UMI_STATUS_INVALID_ARGUMENT; } /* Protect caller-owned memory by checking that required state is available before it is used. */ if(g->count>=UMI_HELIX_MAX_ITEMS){ return UMI_STATUS_CAPACITY_EXCEEDED; } /* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0;i<g->count;i++) /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(g->ids[i],id)==0){ return UMI_STATUS_ALREADY_EXISTS; } UmiStatus s=umi_helix_copy_text(g->ids[g->count],sizeof(g->ids[g->count]),id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK) g->count++; return s; }
/* Record a prerequisite edge after validating both node indices. */
UmiStatus umi_helix_plan_graph_depend(UmiHelixPlanGraph *g,size_t a,size_t b){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(g==NULL||a>=g->count||b>=g->count||a==b){ return UMI_STATUS_INVALID_ARGUMENT; } /* Protect caller-owned memory by checking that required state is available before it is used. */ if(g->edge_count>=UMI_HELIX_MAX_EDGES){ return UMI_STATUS_CAPACITY_EXCEEDED; } /* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0;i<g->edge_count;i++) /* Protect caller-owned memory by checking that required state is available before it is used. */ if(g->edges[i].from==a&&g->edges[i].to==b){ return UMI_STATUS_ALREADY_EXISTS; } g->edges[g->edge_count++]=(UmiHelixPlanGraphEdge){(uint16_t)a,(uint16_t)b}; return UMI_STATUS_OK; }
/* Use Kahn ordering to detect cycles without recursion or allocation. */
bool umi_helix_plan_graph_has_cycle(const UmiHelixPlanGraph *g){ size_t order[UMI_HELIX_MAX_ITEMS],n=0; return umi_helix_plan_graph_order(g,order,UMI_HELIX_MAX_ITEMS,&n)!=UMI_STATUS_OK; }
/* Topologically order nodes with stable lowest-index tie breaking. */
UmiStatus umi_helix_plan_graph_order(const UmiHelixPlanGraph *g,size_t *out,size_t cap,size_t *out_count){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(g==NULL||out==NULL||out_count==NULL||cap<g->count){ return UMI_STATUS_INVALID_ARGUMENT; } size_t indeg[UMI_HELIX_MAX_ITEMS]={0}; bool used[UMI_HELIX_MAX_ITEMS]={false}; /* Visit each bounded item once so every record receives the same rule. */ for(size_t e=0;e<g->edge_count;e++) indeg[g->edges[e].to]++; size_t n=0; /* Continue only while work remains available; the loop body advances the state on each pass. */ while(n<g->count){ size_t pick=g->count; /* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0;i<g->count;i++) /* Protect caller-owned memory by checking that required state is available before it is used. */ if(!used[i]&&indeg[i]==0U){pick=i;break;} /* Protect caller-owned memory by checking that required state is available before it is used. */ if(pick==g->count){*out_count=n;return UMI_STATUS_INVALID_STATE;} used[pick]=true; out[n++]=pick; /* Visit each bounded item once so every record receives the same rule. */ for(size_t e=0;e<g->edge_count;e++) /* Protect caller-owned memory by checking that required state is available before it is used. */ if(g->edges[e].from==pick&&indeg[g->edges[e].to]>0U) indeg[g->edges[e].to]--; } *out_count=n; return UMI_STATUS_OK; }
