/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/helix/regeneration_graph.c
 *
 * PURPOSE:
 *   Track dependencies among regeneration units and reject cyclic regeneration plans.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/regeneration_graph.h"
#include <string.h>
/* Clear all nodes and edges. */
void umi_helix_regeneration_graph_init(UmiHelixRegenerationGraph *g){ if(g!=NULL) memset(g,0,sizeof(*g)); }
/* Add one unique bounded identifier. */
UmiStatus umi_helix_regeneration_graph_add(UmiHelixRegenerationGraph *g,const char *id){ if(g==NULL||!umi_helix_valid_id(id)){ return UMI_STATUS_INVALID_ARGUMENT; } if(g->count>=UMI_HELIX_MAX_ITEMS){ return UMI_STATUS_CAPACITY_EXCEEDED; } for(size_t i=0;i<g->count;i++) if(strcmp(g->ids[i],id)==0){ return UMI_STATUS_ALREADY_EXISTS; } UmiStatus s=umi_helix_copy_text(g->ids[g->count],sizeof(g->ids[g->count]),id); if(s==UMI_STATUS_OK) g->count++; return s; }
/* Record a prerequisite edge after validating both node indices. */
UmiStatus umi_helix_regeneration_graph_depend(UmiHelixRegenerationGraph *g,size_t a,size_t b){ if(g==NULL||a>=g->count||b>=g->count||a==b){ return UMI_STATUS_INVALID_ARGUMENT; } if(g->edge_count>=UMI_HELIX_MAX_EDGES){ return UMI_STATUS_CAPACITY_EXCEEDED; } for(size_t i=0;i<g->edge_count;i++) if(g->edges[i].from==a&&g->edges[i].to==b){ return UMI_STATUS_ALREADY_EXISTS; } g->edges[g->edge_count++]=(UmiHelixRegenerationGraphEdge){(uint16_t)a,(uint16_t)b}; return UMI_STATUS_OK; }
/* Use Kahn ordering to detect cycles without recursion or allocation. */
bool umi_helix_regeneration_graph_has_cycle(const UmiHelixRegenerationGraph *g){ size_t order[UMI_HELIX_MAX_ITEMS],n=0; return umi_helix_regeneration_graph_order(g,order,UMI_HELIX_MAX_ITEMS,&n)!=UMI_STATUS_OK; }
/* Topologically order nodes with stable lowest-index tie breaking. */
UmiStatus umi_helix_regeneration_graph_order(const UmiHelixRegenerationGraph *g,size_t *out,size_t cap,size_t *out_count){ if(g==NULL||out==NULL||out_count==NULL||cap<g->count){ return UMI_STATUS_INVALID_ARGUMENT; } size_t indeg[UMI_HELIX_MAX_ITEMS]={0}; bool used[UMI_HELIX_MAX_ITEMS]={false}; for(size_t e=0;e<g->edge_count;e++) indeg[g->edges[e].to]++; size_t n=0; while(n<g->count){ size_t pick=g->count; for(size_t i=0;i<g->count;i++) if(!used[i]&&indeg[i]==0U){pick=i;break;} if(pick==g->count){*out_count=n;return UMI_STATUS_INVALID_STATE;} used[pick]=true; out[n++]=pick; for(size_t e=0;e<g->edge_count;e++) if(g->edges[e].from==pick&&indeg[g->edges[e].to]>0U) indeg[g->edges[e].to]--; } *out_count=n; return UMI_STATUS_OK; }
