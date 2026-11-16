/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_workflow/memory_plan.c
 *
 * PURPOSE:
 *   Reuse canonical workflow ordering to account for live results and release only after the last selected consumer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai_workflow/memory_plan.h"
#include <stdlib.h>
#include <string.h>

static UmiStatus memory_refuse(UmiAiWorkflowMemoryDiagnostic *diagnostic,
    UmiAiWorkflowMemoryIssue issue,UmiStatus status,size_t node,size_t port)
{
    diagnostic->issue=issue;
    diagnostic->node_index=node;
    diagnostic->port_index=port;
    return status;
}
static bool memory_add(uint64_t *total,uint64_t amount)
{
    if(amount>UINT64_MAX-*total) return false;
    *total+=amount;
    return true;
}
static UmiStatus memory_costs(const UmiAiWorkflowDescription *description,
    const UmiAiWorkflowNodeMemory *costs,const UmiAiWorkflowMemoryPolicy *policy,
    UmiAiWorkflowMemoryDiagnostic *diagnostic)
{
    if(!policy->resident_known && policy->resident_bytes!=0U)
        return memory_refuse(diagnostic,UMI_AI_WORKFLOW_MEMORY_ISSUE_COST,
            UMI_STATUS_INVALID_ARGUMENT,SIZE_MAX,SIZE_MAX);
    for(size_t n=0U;n<description->node_count;++n) {
        const UmiAiWorkflowNodeMemory *cost=&costs[n];
        size_t ports=description->types[description->nodes[n].type_index].output_count;
        uint32_t valid=(UINT32_C(1)<<ports)-UINT32_C(1);
        if((cost->known_outputs & ~valid)!=0U ||
           (!cost->scratch_known && cost->scratch_bytes!=0U))
            return memory_refuse(diagnostic,UMI_AI_WORKFLOW_MEMORY_ISSUE_COST,
                UMI_STATUS_INVALID_ARGUMENT,n,SIZE_MAX);
        for(size_t p=0U;p<UMI_AI_WORKFLOW_MAX_PORTS;++p)
            if((p>=ports || (cost->known_outputs & (UINT32_C(1)<<p))==0U) &&
               cost->output_bytes[p]!=0U)
                return memory_refuse(diagnostic,UMI_AI_WORKFLOW_MEMORY_ISSUE_COST,
                    UMI_STATUS_INVALID_ARGUMENT,n,p);
    }
    return UMI_STATUS_OK;
}
static void memory_lifetimes(const UmiAiWorkflowDescription *description,
    const UmiAiWorkflowNodeMemory *costs,UmiAiWorkflowMemoryPlan *plan)
{
    size_t positions[UMI_AI_WORKFLOW_MAX_NODES];
    bool retained[UMI_AI_WORKFLOW_MAX_NODES]={false};
    for(size_t n=0U;n<UMI_AI_WORKFLOW_MAX_NODES;++n) positions[n]=SIZE_MAX;
    for(size_t i=0U;i<plan->workflow.order.count;++i)
        positions[plan->workflow.node_indices[i]]=i;
    for(size_t i=0U;i<description->output_count;++i)
        retained[description->output_nodes[i]]=true;
    for(size_t i=0U;i<plan->workflow.order.count;++i) {
        size_t n=plan->workflow.node_indices[i];
        size_t ports=description->types[description->nodes[n].type_index].output_count;
        if(!costs[n].scratch_known) plan->estimates_complete=false;
        for(size_t p=0U;p<ports;++p) {
            UmiAiWorkflowValueLifetime *value=&plan->values[plan->value_count++];
            value->node_index=n;value->port_index=p;value->first_step=i;
            value->last_step=retained[n]?plan->workflow.order.count-1U:i;
            value->retained_output=retained[n];
            value->bytes_known=(costs[n].known_outputs & (UINT32_C(1)<<p))!=0U;
            value->bytes=costs[n].output_bytes[p];
            if(!value->bytes_known) plan->estimates_complete=false;
            /* A consumer may read one owner through several input links. Its
             * position extends lifetime; it never duplicates that allocation. */
            for(size_t e=0U;e<description->link_count;++e) {
                const UmiAiWorkflowLink *link=&description->links[e];
                size_t consumer=positions[link->target_node];
                if(link->source_node==n && link->source_port==p &&
                   consumer!=SIZE_MAX && consumer>value->last_step)
                    value->last_step=consumer;
            }
        }
    }
}
static UmiStatus memory_totals(const UmiAiWorkflowNodeMemory *costs,
    const UmiAiWorkflowMemoryPolicy *policy,UmiAiWorkflowMemoryPlan *plan,
    UmiAiWorkflowMemoryDiagnostic *diagnostic)
{
    uint64_t live=policy->resident_bytes;
    plan->peak_bytes=live;plan->peak_step=SIZE_MAX;
    for(size_t i=0U;i<plan->workflow.order.count;++i) {
        UmiAiWorkflowMemoryStep *step=&plan->steps[i];
        size_t n=plan->workflow.node_indices[i];
        step->node_index=n;step->live_before_bytes=live;
        step->scratch_bytes=costs[n].scratch_bytes;
        for(size_t v=0U;v<plan->value_count;++v) {
            const UmiAiWorkflowValueLifetime *value=&plan->values[v];
            if(value->first_step!=i) continue;
            if(!memory_add(&step->output_bytes,value->bytes) ||
               (value->retained_output && !memory_add(&plan->retained_bytes,value->bytes)))
                return memory_refuse(diagnostic,UMI_AI_WORKFLOW_MEMORY_ISSUE_OVERFLOW,
                    UMI_STATUS_CAPACITY_EXCEEDED,n,value->port_index);
        }
        if(!memory_add(&live,step->output_bytes))
            return memory_refuse(diagnostic,UMI_AI_WORKFLOW_MEMORY_ISSUE_OVERFLOW,
                UMI_STATUS_CAPACITY_EXCEEDED,n,SIZE_MAX);
        step->peak_bytes=live;
        if(!memory_add(&step->peak_bytes,step->scratch_bytes))
            return memory_refuse(diagnostic,UMI_AI_WORKFLOW_MEMORY_ISSUE_OVERFLOW,
                UMI_STATUS_CAPACITY_EXCEEDED,n,SIZE_MAX);
        if(step->peak_bytes>plan->peak_bytes) {
            plan->peak_bytes=step->peak_bytes;plan->peak_step=i;
        }
        for(size_t v=0U;v<plan->value_count;++v) {
            const UmiAiWorkflowValueLifetime *value=&plan->values[v];
            if(value->last_step==i && !value->retained_output) {
                /* Only known bytes that were previously added are released.
                 * The bounded validated order prevents unsigned underflow. */
                live-=value->bytes;
                step->released_bytes+=value->bytes;
            }
        }
        step->live_after_bytes=live;
    }
    plan->final_live_bytes=live;
    if(!plan->estimates_complete) plan->verdict=UMI_AI_WORKFLOW_MEMORY_INCOMPLETE;
    else if(plan->peak_bytes>policy->byte_limit) {
        plan->verdict=UMI_AI_WORKFLOW_MEMORY_EXCEEDS_LIMIT;
        plan->deficit_bytes=plan->peak_bytes-policy->byte_limit;
    } else {
        plan->verdict=UMI_AI_WORKFLOW_MEMORY_WITHIN_DECLARED_LIMIT;
        plan->remaining_bytes=policy->byte_limit-plan->peak_bytes;
    }
    return UMI_STATUS_OK;
}
UmiStatus umi_ai_workflow_memory_plan(const UmiAiWorkflowDescription *description,
    const UmiAiWorkflowNodeMemory *costs,size_t cost_count,
    const UmiAiWorkflowMemoryPolicy *policy,uint32_t allowed_effects,
    UmiAiWorkflowMemoryPlan *out_plan,UmiAiWorkflowMemoryDiagnostic *diagnostic)
{
    if(diagnostic==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *diagnostic=(UmiAiWorkflowMemoryDiagnostic){.issue=UMI_AI_WORKFLOW_MEMORY_ISSUE_NONE,
        .node_index=SIZE_MAX,.port_index=SIZE_MAX,
        .workflow={UMI_AI_WORKFLOW_ISSUE_NONE,SIZE_MAX,SIZE_MAX,SIZE_MAX,SIZE_MAX}};
    if(description==NULL || costs==NULL || policy==NULL || out_plan==NULL ||
       cost_count!=description->node_count)
        return memory_refuse(diagnostic,UMI_AI_WORKFLOW_MEMORY_ISSUE_ARGUMENT,
            UMI_STATUS_INVALID_ARGUMENT,SIZE_MAX,SIZE_MAX);
    UmiAiWorkflowMemoryPlan *candidate=calloc(1U,sizeof(*candidate));
    if(candidate==NULL)
        return memory_refuse(diagnostic,UMI_AI_WORKFLOW_MEMORY_ISSUE_ALLOCATION,
            UMI_STATUS_OUT_OF_MEMORY,SIZE_MAX,SIZE_MAX);
    UmiStatus status=umi_ai_workflow_preflight(description,allowed_effects,
        &candidate->workflow,&diagnostic->workflow);
    if(status!=UMI_STATUS_OK) diagnostic->issue=UMI_AI_WORKFLOW_MEMORY_ISSUE_WORKFLOW;
    else status=memory_costs(description,costs,policy,diagnostic);
    if(status==UMI_STATUS_OK) {
        candidate->estimates_complete=policy->resident_known;
        memory_lifetimes(description,costs,candidate);
        status=memory_totals(costs,policy,candidate,diagnostic);
    }
    /* Publish only complete reports, including honest INCOMPLETE assessments.
     * No caller borrows workspace storage and all error paths converge here. */
    if(status==UMI_STATUS_OK) memcpy(out_plan,candidate,sizeof(*out_plan));
    free(candidate);
    return status;
}
