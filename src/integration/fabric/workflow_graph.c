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


/* Provide the wf index operation used by this module and its client applications. */
static int wf_index(const UmiFabricWorkflowGraph *g,const char *id){size_t i;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<g->step_count;++i)/* Use the stable identifier comparison to choose the matching record or policy. */ if(strcmp(g->steps[i].step_id,id)==0)return (int)i;return -1;}
/* Provide the wf dfs operation used by this module and its client applications. */
static bool wf_dfs(const UmiFabricWorkflowGraph *g,size_t idx,unsigned char *mark){size_t e;mark[idx]=1U;/* Visit each bounded item once so every record receives the same rule. */ for(e=0U;e<g->edge_count;++e)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(g->edges[e].before_id,g->steps[idx].step_id)==0){int j=wf_index(g,g->edges[e].after_id);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(j>=0){size_t k=(size_t)j;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(mark[k]==1U)return true;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(mark[k]==0U&&wf_dfs(g,k,mark))return true;}}mark[idx]=2U;return false;}

/* Public graphs can be restored or filled by an SDK caller. Validate every live
 * record before comparisons or traversal; unused capacity is not part of state.
 * Repeated edges remain valid for compatibility with existing dependency lists. */
UmiStatus umi_fabric_workflow_graph_validate(const UmiFabricWorkflowGraph *graph)
{
    if (graph == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (graph->step_count > UMI_FABRIC_MAX_STEPS ||
        graph->edge_count > UMI_FABRIC_MAX_EDGES) return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < graph->step_count; ++i)
        if (umi_fabric_workflow_step_validate(&graph->steps[i]) != UMI_STATUS_OK)
            return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < graph->step_count; ++i)
        for (size_t j = 0U; j < i; ++j)
            if (strcmp(graph->steps[i].step_id, graph->steps[j].step_id) == 0)
                return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < graph->edge_count; ++i) {
        const UmiFabricWorkflowEdge *edge = &graph->edges[i];
        if (memchr(edge->before_id, 0, sizeof(edge->before_id)) == NULL ||
            memchr(edge->after_id, 0, sizeof(edge->after_id)) == NULL ||
            edge->before_id[0] == '\0' || edge->after_id[0] == '\0')
            return UMI_STATUS_INVALID_STATE;
        if (wf_index(graph, edge->before_id) < 0 || wf_index(graph, edge->after_id) < 0)
            return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

/* Checked queries preserve the caller's previous answer on malformed storage.
 * A structurally valid cycle is a result, not a parser or memory-safety error. */
UmiStatus umi_fabric_workflow_graph_has_cycle_checked(
    const UmiFabricWorkflowGraph *graph, bool *out_cycle)
{
    if (out_cycle == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_fabric_workflow_graph_validate(graph);
    if (status != UMI_STATUS_OK) return status;
    unsigned char mark[UMI_FABRIC_MAX_STEPS] = {0};
    bool cycle = false;
    for (size_t i = 0U; i < graph->step_count && !cycle; ++i)
        if (mark[i] == 0U) cycle = wf_dfs(graph, i, mark);
    *out_cycle = cycle;
    return UMI_STATUS_OK;
}

/* An edge before->after is cyclic exactly when after already reaches before.
 * This bounded search runs on validated storage and never writes a tentative
 * edge into the live graph, including its currently unused capacity. */
static bool wf_reaches(const UmiFabricWorkflowGraph *graph, size_t from, size_t to)
{
    size_t queue[UMI_FABRIC_MAX_STEPS];
    bool seen[UMI_FABRIC_MAX_STEPS] = {false};
    size_t count = 1U, next = 0U;
    queue[0] = from;
    seen[from] = true;
    while (next < count) {
        size_t current = queue[next++];
        if (current == to) return true;
        for (size_t e = 0U; e < graph->edge_count; ++e) {
            if (strcmp(graph->steps[current].step_id, graph->edges[e].before_id) != 0)
                continue;
            size_t target = (size_t)wf_index(graph, graph->edges[e].after_id);
            if (!seen[target]) {
                seen[target] = true;
                queue[count++] = target;
            }
        }
    }
    return false;
}

/*
 * Initialise fabric workflow graph from caller-provided values so later operations receive
 * a known state.
 */
void umi_fabric_workflow_graph_init(UmiFabricWorkflowGraph *graph){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(graph!=NULL)(void)memset(graph,0,sizeof(*graph));}
/*
 * Provide the fabric workflow graph add step operation used by this module and its client
 * applications.
 */
/* Validate the complete graph before traversal and publish only accepted changes, so malformed SDK state and refused edges cannot alter or overrun live storage. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_fabric_workflow_graph_add_step(UmiFabricWorkflowGraph *graph,const UmiFabricWorkflowStep *step){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(graph==NULL||step==NULL||umi_fabric_workflow_step_validate(step)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<graph->step_count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(graph->steps[i].step_id,step->step_id)==0)return UMI_STATUS_ALREADY_EXISTS;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(graph->step_count>=UMI_FABRIC_MAX_STEPS)return UMI_STATUS_CAPACITY_EXCEEDED;graph->steps[graph->step_count++]=*step;return UMI_STATUS_OK;}
#endif
UmiStatus umi_fabric_workflow_graph_add_step(UmiFabricWorkflowGraph *graph,
    const UmiFabricWorkflowStep *step)
{
    if (graph == NULL || step == NULL ||
        umi_fabric_workflow_step_validate(step) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    bool cycle;
    UmiStatus status = umi_fabric_workflow_graph_has_cycle_checked(graph, &cycle);
    if (status != UMI_STATUS_OK) return status;
    if (cycle) return UMI_STATUS_INVALID_STATE;
    if (wf_index(graph, step->step_id) >= 0) return UMI_STATUS_ALREADY_EXISTS;
    if (graph->step_count == UMI_FABRIC_MAX_STEPS) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* The source may be an unused slot in this same graph. Stage it before
     * publishing the slot and incrementing the live-record count. */
    UmiFabricWorkflowStep candidate = *step;
    graph->steps[graph->step_count] = candidate;
    ++graph->step_count;
    return UMI_STATUS_OK;
}
/*
 * Provide the fabric workflow graph add edge operation used by this module and its client
 * applications.
 */
/* Validate the complete graph before traversal and publish only accepted changes, so malformed SDK state and refused edges cannot alter or overrun live storage. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_fabric_workflow_graph_add_edge(UmiFabricWorkflowGraph *graph,const char *before_id,const char *after_id){UmiFabricWorkflowEdge *e;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(graph==NULL||before_id==NULL||after_id==NULL||strcmp(before_id,after_id)==0)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(wf_index(graph,before_id)<0||wf_index(graph,after_id)<0)return UMI_STATUS_NOT_FOUND;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(graph->edge_count>=UMI_FABRIC_MAX_EDGES)return UMI_STATUS_CAPACITY_EXCEEDED;e=&graph->edges[graph->edge_count++];(void)umi_fabric_copy_text(e->before_id,sizeof(e->before_id),before_id);(void)umi_fabric_copy_text(e->after_id,sizeof(e->after_id),after_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_fabric_workflow_graph_has_cycle(graph)){graph->edge_count--;return UMI_STATUS_INVALID_STATE;}return UMI_STATUS_OK;}
#endif
UmiStatus umi_fabric_workflow_graph_add_edge(UmiFabricWorkflowGraph *graph,
    const char *before_id, const char *after_id)
{
    if (graph == NULL || before_id == NULL || after_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    bool cycle;
    UmiStatus status = umi_fabric_workflow_graph_has_cycle_checked(graph, &cycle);
    if (status != UMI_STATUS_OK) return status;
    if (cycle) return UMI_STATUS_INVALID_STATE;
    if (strcmp(before_id, after_id) == 0) return UMI_STATUS_INVALID_ARGUMENT;
    int before = wf_index(graph, before_id), after = wf_index(graph, after_id);
    if (before < 0 || after < 0) return UMI_STATUS_NOT_FOUND;
    if (graph->edge_count == UMI_FABRIC_MAX_EDGES) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (wf_reaches(graph, (size_t)after, (size_t)before)) return UMI_STATUS_INVALID_STATE;
    UmiFabricWorkflowEdge candidate = {0};
    status = umi_fabric_copy_text(candidate.before_id, sizeof(candidate.before_id), before_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_fabric_copy_text(candidate.after_id, sizeof(candidate.after_id), after_id);
    if (status != UMI_STATUS_OK) return status;
    graph->edges[graph->edge_count] = candidate;
    ++graph->edge_count;
    return UMI_STATUS_OK;
}
/*
 * Provide the fabric workflow graph has cycle operation used by this module and its client
 * applications.
 */
/* Validate the complete graph before traversal and publish only accepted changes, so malformed SDK state and refused edges cannot alter or overrun live storage. The former implementation is retained for owner review. */
#if 0
bool umi_fabric_workflow_graph_has_cycle(const UmiFabricWorkflowGraph *graph){unsigned char mark[UMI_FABRIC_MAX_STEPS]={0};size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(graph==NULL)return false;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<graph->step_count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(mark[i]==0U&&wf_dfs(graph,i,mark))return true;return false;}
#endif
bool umi_fabric_workflow_graph_has_cycle(const UmiFabricWorkflowGraph *graph)
{
    if (graph == NULL) return false; /* Retain the original null-query convention. */
    bool cycle;
    UmiStatus status = umi_fabric_workflow_graph_has_cycle_checked(graph, &cycle);
    /* Legacy callers have no status channel. Refuse malformed non-null graphs
     * conservatively; use the checked query to distinguish invalid from cyclic. */
    return status != UMI_STATUS_OK || cycle;
}
