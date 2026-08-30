/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/workflow_plan.c
 *
 * PURPOSE:
 *   Topologically order workflow steps so dependencies are always executed first.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/workflow_plan.h"
#include <string.h>
#include <limits.h>


UmiStatus umi_fabric_workflow_plan_build(const UmiFabricWorkflowGraph *graph,UmiFabricWorkflowPlan *out_plan){size_t indegree[UMI_FABRIC_MAX_STEPS]={0};bool used[UMI_FABRIC_MAX_STEPS]={false};size_t e,i,k;if(graph==NULL||out_plan==NULL||graph->step_count==0U||umi_fabric_workflow_graph_has_cycle(graph))return UMI_STATUS_INVALID_ARGUMENT;(void)memset(out_plan,0,sizeof(*out_plan));for(e=0U;e<graph->edge_count;++e){for(i=0U;i<graph->step_count;++i)if(strcmp(graph->steps[i].step_id,graph->edges[e].after_id)==0){indegree[i]++;break;}}for(k=0U;k<graph->step_count;++k){size_t best=graph->step_count;for(i=0U;i<graph->step_count;++i)if(!used[i]&&indegree[i]==0U&&(best==graph->step_count||strcmp(graph->steps[i].step_id,graph->steps[best].step_id)<0))best=i;if(best==graph->step_count)return UMI_STATUS_INVALID_STATE;used[best]=true;(void)umi_fabric_copy_text(out_plan->step_ids[out_plan->count],UMI_FABRIC_ID_CAPACITY,graph->steps[best].step_id);out_plan->fingerprint=umi_fabric_hash64(graph->steps[best].step_id,strlen(graph->steps[best].step_id),out_plan->fingerprint);out_plan->count++;for(e=0U;e<graph->edge_count;++e)if(strcmp(graph->edges[e].before_id,graph->steps[best].step_id)==0){for(i=0U;i<graph->step_count;++i)if(strcmp(graph->steps[i].step_id,graph->edges[e].after_id)==0&&indegree[i]>0U){indegree[i]--;break;}}}return UMI_STATUS_OK;}
