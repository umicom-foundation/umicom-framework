/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/migration_plan.c
 *
 * PURPOSE:
 *   Produce a deterministic topological migration order from a validated dependency graph.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/migration_plan.h"
#include <string.h>

/* Kahn-style ordering keeps migration execution deterministic and reviewable. */ UmiStatus umi_data_migration_plan_build(const UmiDataMigrationGraph *graph,UmiDataMigrationPlan *out_plan){size_t indegree[UMI_DATA_ENTERPRISE_MAX_ITEMS]={0};bool emitted[UMI_DATA_ENTERPRISE_MAX_ITEMS]={0};size_t i,j;if(graph==NULL||out_plan==NULL||graph->step_count==0U)return UMI_STATUS_INVALID_ARGUMENT;if(umi_data_migration_graph_has_cycle(graph))return UMI_STATUS_INVALID_STATE;(void)memset(out_plan,0,sizeof(*out_plan));out_plan->reversible=true;for(i=0;i<graph->edge_count;++i)for(j=0;j<graph->step_count;++j)if(strcmp(graph->edges[i].to_step,graph->steps[j].step_id)==0)indegree[j]++;while(out_plan->count<graph->step_count){bool progress=false;for(i=0;i<graph->step_count;++i)if(!emitted[i]&&indegree[i]==0U){(void)umi_data_enterprise_copy_text(out_plan->step_ids[out_plan->count],sizeof(out_plan->step_ids[out_plan->count]),graph->steps[i].step_id);out_plan->count++;emitted[i]=true;progress=true;if(!graph->steps[i].reversible)out_plan->reversible=false;if(graph->steps[i].destructive)out_plan->destructive=true;for(j=0;j<graph->edge_count;++j)if(strcmp(graph->edges[j].from_step,graph->steps[i].step_id)==0){size_t k;for(k=0;k<graph->step_count;++k)if(strcmp(graph->edges[j].to_step,graph->steps[k].step_id)==0&&indegree[k]>0U)indegree[k]--;}}if(!progress)return UMI_STATUS_INVALID_STATE;}return UMI_STATUS_OK;}
