/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/workflow_graph.c
 *
 * PURPOSE:
 *   Maintain workflow dependency edges and reject cycles before orchestration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/workflow_graph.h"
#include <string.h>
#include <limits.h>


static int wf_index(const UmiFabricWorkflowGraph *g,const char *id){size_t i;for(i=0U;i<g->step_count;++i)if(strcmp(g->steps[i].step_id,id)==0)return (int)i;return -1;}
static bool wf_dfs(const UmiFabricWorkflowGraph *g,size_t idx,unsigned char *mark){size_t e;mark[idx]=1U;for(e=0U;e<g->edge_count;++e)if(strcmp(g->edges[e].before_id,g->steps[idx].step_id)==0){int j=wf_index(g,g->edges[e].after_id);if(j>=0){size_t k=(size_t)j;if(mark[k]==1U)return true;if(mark[k]==0U&&wf_dfs(g,k,mark))return true;}}mark[idx]=2U;return false;}
void umi_fabric_workflow_graph_init(UmiFabricWorkflowGraph *graph){if(graph!=NULL)(void)memset(graph,0,sizeof(*graph));}
UmiStatus umi_fabric_workflow_graph_add_step(UmiFabricWorkflowGraph *graph,const UmiFabricWorkflowStep *step){size_t i;if(graph==NULL||step==NULL||umi_fabric_workflow_step_validate(step)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<graph->step_count;++i)if(strcmp(graph->steps[i].step_id,step->step_id)==0)return UMI_STATUS_ALREADY_EXISTS;if(graph->step_count>=UMI_FABRIC_MAX_STEPS)return UMI_STATUS_CAPACITY_EXCEEDED;graph->steps[graph->step_count++]=*step;return UMI_STATUS_OK;}
UmiStatus umi_fabric_workflow_graph_add_edge(UmiFabricWorkflowGraph *graph,const char *before_id,const char *after_id){UmiFabricWorkflowEdge *e;if(graph==NULL||before_id==NULL||after_id==NULL||strcmp(before_id,after_id)==0)return UMI_STATUS_INVALID_ARGUMENT;if(wf_index(graph,before_id)<0||wf_index(graph,after_id)<0)return UMI_STATUS_NOT_FOUND;if(graph->edge_count>=UMI_FABRIC_MAX_EDGES)return UMI_STATUS_CAPACITY_EXCEEDED;e=&graph->edges[graph->edge_count++];(void)umi_fabric_copy_text(e->before_id,sizeof(e->before_id),before_id);(void)umi_fabric_copy_text(e->after_id,sizeof(e->after_id),after_id);if(umi_fabric_workflow_graph_has_cycle(graph)){graph->edge_count--;return UMI_STATUS_INVALID_STATE;}return UMI_STATUS_OK;}
bool umi_fabric_workflow_graph_has_cycle(const UmiFabricWorkflowGraph *graph){unsigned char mark[UMI_FABRIC_MAX_STEPS]={0};size_t i;if(graph==NULL)return false;for(i=0U;i<graph->step_count;++i)if(mark[i]==0U&&wf_dfs(graph,i,mark))return true;return false;}
