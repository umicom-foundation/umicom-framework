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
#include "../../base/value_construction_internal.h"
#include <string.h>
#include <limits.h>


/*
 * Provide the fabric workflow plan build operation used by this module and its client
 * applications.
 */
/* The shared graph validator now admits complete bounded storage, and staged plan construction preserves earlier output on failure while keeping deterministic ordering. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_fabric_workflow_plan_build(const UmiFabricWorkflowGraph *graph,UmiFabricWorkflowPlan *out_plan){size_t indegree[UMI_FABRIC_MAX_STEPS]={0};bool used[UMI_FABRIC_MAX_STEPS]={false};size_t e,i,k;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(graph==NULL||out_plan==NULL||graph->step_count==0U||umi_fabric_workflow_graph_has_cycle(graph))return UMI_STATUS_INVALID_ARGUMENT;(void)memset(out_plan,0,sizeof(*out_plan));/* Visit each bounded item once so every record receives the same rule. */ for(e=0U;e<graph->edge_count;++e){/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<graph->step_count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(graph->steps[i].step_id,graph->edges[e].after_id)==0){indegree[i]++;break;}}/* Visit each bounded item once so every record receives the same rule. */ for(k=0U;k<graph->step_count;++k){size_t best=graph->step_count;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<graph->step_count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!used[i]&&indegree[i]==0U&&(best==graph->step_count||strcmp(graph->steps[i].step_id,graph->steps[best].step_id)<0))best=i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(best==graph->step_count)return UMI_STATUS_INVALID_STATE;used[best]=true;(void)umi_fabric_copy_text(out_plan->step_ids[out_plan->count],UMI_FABRIC_ID_CAPACITY,graph->steps[best].step_id);out_plan->fingerprint=umi_fabric_hash64(graph->steps[best].step_id,strlen(graph->steps[best].step_id),out_plan->fingerprint);out_plan->count++;/* Visit each bounded item once so every record receives the same rule. */ for(e=0U;e<graph->edge_count;++e)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(graph->edges[e].before_id,graph->steps[best].step_id)==0){/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<graph->step_count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(graph->steps[i].step_id,graph->edges[e].after_id)==0&&indegree[i]>0U){indegree[i]--;break;}}}return UMI_STATUS_OK;}
#endif
/* Build into the shared constructor's temporary object. Every input remains
 * readable until final publication; the caller's earlier plan survives errors. */
static UmiStatus umi_fabric_workflow_plan_build_value(
    const UmiFabricWorkflowGraph *graph, UmiFabricWorkflowPlan *candidate)
{
    if (graph == NULL || candidate == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    bool cycle;
    UmiStatus status = umi_fabric_workflow_graph_has_cycle_checked(graph, &cycle);
    if (status != UMI_STATUS_OK) return status;
    if (graph->step_count == 0U || cycle) return UMI_STATUS_INVALID_ARGUMENT;
    size_t indegree[UMI_FABRIC_MAX_STEPS] = {0};
    bool used[UMI_FABRIC_MAX_STEPS] = {false};
    memset(candidate, 0, sizeof(*candidate));
    for (size_t e = 0U; e < graph->edge_count; ++e)
        for (size_t i = 0U; i < graph->step_count; ++i)
            if (strcmp(graph->steps[i].step_id, graph->edges[e].after_id) == 0) {
                ++indegree[i];
                break;
            }
    for (size_t k = 0U; k < graph->step_count; ++k) {
        size_t best = graph->step_count;
        for (size_t i = 0U; i < graph->step_count; ++i)
            if (!used[i] && indegree[i] == 0U &&
                (best == graph->step_count ||
                 strcmp(graph->steps[i].step_id, graph->steps[best].step_id) < 0)) best = i;
        if (best == graph->step_count) return UMI_STATUS_INVALID_STATE;
        used[best] = true;
        status = umi_fabric_copy_text(candidate->step_ids[candidate->count],
            UMI_FABRIC_ID_CAPACITY, graph->steps[best].step_id);
        if (status != UMI_STATUS_OK) return status;
        /* Preserve the original order-only fingerprint. This non-cryptographic
         * value is not an approval token or an identity for settings/edges. */
        candidate->fingerprint = umi_fabric_hash64(graph->steps[best].step_id,
            strlen(graph->steps[best].step_id), candidate->fingerprint);
        ++candidate->count;
        for (size_t e = 0U; e < graph->edge_count; ++e)
            if (strcmp(graph->edges[e].before_id, graph->steps[best].step_id) == 0)
                for (size_t i = 0U; i < graph->step_count; ++i)
                    if (strcmp(graph->steps[i].step_id, graph->edges[e].after_id) == 0 &&
                        indegree[i] > 0U) {
                        --indegree[i];
                        break;
                    }
    }
    return UMI_STATUS_OK;
}

UMI_DEFINE_STAGED_VALUE_INIT(umi_fabric_workflow_plan_build, UmiFabricWorkflowPlan, out_plan,
    (const UmiFabricWorkflowGraph *graph, UmiFabricWorkflowPlan *out_plan),
    umi_fabric_workflow_plan_build_value(graph, candidate))
