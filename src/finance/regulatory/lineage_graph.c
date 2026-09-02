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
/* Provide the find node operation used by this module and its client applications. */
static int find_node(const UmiLineageGraph *g,const char *id){ size_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(g==NULL||id==NULL)return -1; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<g->node_count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(g->node_ids[i],id)==0)return (int)i; return -1; }
/* Provide the reachable operation used by this module and its client applications. */
static int reachable(const UmiLineageGraph *g,size_t from,size_t target,int *seen){ size_t i; /* Apply this branch only when its contract condition is satisfied. */ if(from==target)return 1; /* Apply this branch only when its contract condition is satisfied. */ if(seen[from]!=0)return 0; seen[from]=1; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<g->edge_count;++i)/* Apply this branch only when its contract condition is satisfied. */ if(g->edges[i].from_index==from&&reachable(g,g->edges[i].to_index,target,seen))return 1; return 0; }
/*
 * Initialise reg lineage graph from caller-provided values so later operations receive a
 * known state.
 */
void umi_reg_lineage_graph_init(UmiLineageGraph *g){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(g!=NULL)memset(g,0,sizeof *g); }
/*
 * Provide the reg lineage graph add node operation used by this module and its client
 * applications.
 */
UmiStatus umi_reg_lineage_graph_add_node(UmiLineageGraph *g,const char *id){ UmiStatus s; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(g==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(find_node(g,id)>=0)return UMI_STATUS_ALREADY_EXISTS; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(g->node_count>=UMI_REG_MAX_NODES)return UMI_STATUS_CAPACITY_EXCEEDED; s=umi_reg_copy_text(g->node_ids[g->node_count],UMI_REG_ID_CAPACITY,id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)++g->node_count; return s; }
/*
 * Provide the reg lineage graph add edge operation used by this module and its client
 * applications.
 */
UmiStatus umi_reg_lineage_graph_add_edge(UmiLineageGraph *g,const char *from,const char *to){ int a,b,seen[UMI_REG_MAX_NODES]={0}; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(g==NULL)return UMI_STATUS_INVALID_ARGUMENT; a=find_node(g,from);b=find_node(g,to);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(a<0||b<0)return UMI_STATUS_NOT_FOUND;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(a==b)return UMI_STATUS_INVALID_STATE;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(g->edge_count>=UMI_REG_MAX_EDGES)return UMI_STATUS_CAPACITY_EXCEEDED;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(reachable(g,(size_t)b,(size_t)a,seen))return UMI_STATUS_INVALID_STATE;g->edges[g->edge_count].from_index=(size_t)a;g->edges[g->edge_count].to_index=(size_t)b;++g->edge_count;return UMI_STATUS_OK; }
