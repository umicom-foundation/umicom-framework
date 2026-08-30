/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/lineage_graph.c
 *
 * PURPOSE:
 *   Maintain a bounded acyclic data-lineage graph with cycle prevention.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/regulatory/lineage_graph.h"

#include <string.h>
static int find_node(const UmiLineageGraph *g,const char *id){ size_t i; if(g==NULL||id==NULL)return -1; for(i=0U;i<g->node_count;++i)if(strcmp(g->node_ids[i],id)==0)return (int)i; return -1; }
static int reachable(const UmiLineageGraph *g,size_t from,size_t target,int *seen){ size_t i; if(from==target)return 1; if(seen[from]!=0)return 0; seen[from]=1; for(i=0U;i<g->edge_count;++i)if(g->edges[i].from_index==from&&reachable(g,g->edges[i].to_index,target,seen))return 1; return 0; }
void umi_reg_lineage_graph_init(UmiLineageGraph *g){ if(g!=NULL)memset(g,0,sizeof *g); }
UmiStatus umi_reg_lineage_graph_add_node(UmiLineageGraph *g,const char *id){ UmiStatus s; if(g==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT; if(find_node(g,id)>=0)return UMI_STATUS_ALREADY_EXISTS; if(g->node_count>=UMI_REG_MAX_NODES)return UMI_STATUS_CAPACITY_EXCEEDED; s=umi_reg_copy_text(g->node_ids[g->node_count],UMI_REG_ID_CAPACITY,id); if(s==UMI_STATUS_OK)++g->node_count; return s; }
UmiStatus umi_reg_lineage_graph_add_edge(UmiLineageGraph *g,const char *from,const char *to){ int a,b,seen[UMI_REG_MAX_NODES]={0}; if(g==NULL)return UMI_STATUS_INVALID_ARGUMENT; a=find_node(g,from);b=find_node(g,to);if(a<0||b<0)return UMI_STATUS_NOT_FOUND;if(a==b)return UMI_STATUS_INVALID_STATE;if(g->edge_count>=UMI_REG_MAX_EDGES)return UMI_STATUS_CAPACITY_EXCEEDED;if(reachable(g,(size_t)b,(size_t)a,seen))return UMI_STATUS_INVALID_STATE;g->edges[g->edge_count].from_index=(size_t)a;g->edges[g->edge_count].to_index=(size_t)b;++g->edge_count;return UMI_STATUS_OK; }
