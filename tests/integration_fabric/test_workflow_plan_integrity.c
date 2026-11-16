/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_workflow_plan_integrity.c
 *
 * PURPOSE:
 *   Verify deterministic dependency ordering and retained plans across invalid input and allocation failure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/integration/fabric/workflow_plan.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Exercise the actual plan constructor with a private allocation-failure seam.
 * Production graph validation, step validation and hashing remain linked normally. */
static bool fail_allocation;
static size_t allocations, live_allocations;
static void *PlanAllocate(size_t size)
{
    ++allocations;
    if (fail_allocation) return NULL;
    void *result = malloc(size);
    if (result != NULL) ++live_allocations;
    return result;
}
static void PlanFree(void *value)
{
    if (value != NULL) --live_allocations;
    free(value);
}
UmiStatus UmiTestPlanBuild(const UmiFabricWorkflowGraph *graph, UmiFabricWorkflowPlan *out_plan);
#define malloc PlanAllocate
#define free PlanFree
#define umi_fabric_workflow_plan_build UmiTestPlanBuild
#include "../../src/integration/fabric/workflow_plan.c"
#undef umi_fabric_workflow_plan_build
#undef free
#undef malloc

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
static int Add(UmiFabricWorkflowGraph *graph, const char *id)
{
    UmiFabricWorkflowStep step;
    CHECK(umi_fabric_workflow_step_init(&step, id, "operation", 10U, false, false) == UMI_STATUS_OK);
    CHECK(umi_fabric_workflow_graph_add_step(graph, &step) == UMI_STATUS_OK);
    return 0;
}
static int Diamond(UmiFabricWorkflowGraph *graph, bool reverse)
{
    static const char *const ids[] = {"c-sink", "b-right", "a-left", "z-source", "m-alone"};
    umi_fabric_workflow_graph_init(graph);
    for (size_t i = 0U; i < 5U; ++i) CHECK(Add(graph, ids[reverse ? 4U-i : i]) == 0);
    CHECK(umi_fabric_workflow_graph_add_edge(graph, "z-source", "a-left") == UMI_STATUS_OK);
    CHECK(umi_fabric_workflow_graph_add_edge(graph, "z-source", "b-right") == UMI_STATUS_OK);
    CHECK(umi_fabric_workflow_graph_add_edge(graph, "a-left", "c-sink") == UMI_STATUS_OK);
    CHECK(umi_fabric_workflow_graph_add_edge(graph, "b-right", "c-sink") == UMI_STATUS_OK);
    return 0;
}
static int Preserved(UmiFabricWorkflowGraph *graph, UmiFabricWorkflowPlan *plan, UmiStatus expected)
{
    unsigned char saved[sizeof(*plan)];
    memset(plan, 0xa5, sizeof(*plan));
    memcpy(saved, plan, sizeof(saved));
    CHECK(umi_fabric_workflow_plan_build(graph, plan) == expected);
    CHECK(memcmp(saved, plan, sizeof(saved)) == 0);
    return 0;
}
static int Run(const char *mode, UmiFabricWorkflowGraph *graph, UmiFabricWorkflowPlan *plan)
{
    CHECK(Diamond(graph, false) == 0);
    if (strcmp(mode, "deterministic") == 0) {
        const char *const expected[] = {"m-alone", "z-source", "a-left", "b-right", "c-sink"};
        CHECK(umi_fabric_workflow_plan_build(graph, plan) == UMI_STATUS_OK);
        CHECK(plan->count == 5U);
        uint64_t fingerprint = plan->fingerprint;
        for (size_t i = 0U; i < 5U; ++i) CHECK(strcmp(plan->step_ids[i], expected[i]) == 0);
        CHECK(Diamond(graph, true) == 0);
        CHECK(umi_fabric_workflow_plan_build(graph, plan) == UMI_STATUS_OK);
        CHECK(plan->fingerprint == fingerprint);
        for (size_t i = 0U; i < 5U; ++i) CHECK(strcmp(plan->step_ids[i], expected[i]) == 0);
    } else if (strcmp(mode, "invalid-storage") == 0) {
        graph->step_count = SIZE_MAX;
        CHECK(Preserved(graph, plan, UMI_STATUS_INVALID_STATE) == 0);
        CHECK(Diamond(graph, false) == 0);
        graph->edge_count = SIZE_MAX;
        CHECK(Preserved(graph, plan, UMI_STATUS_INVALID_STATE) == 0);
        CHECK(Diamond(graph, false) == 0);
        memset(graph->steps[4].operation_id, 'x', UMI_FABRIC_ID_CAPACITY);
        CHECK(Preserved(graph, plan, UMI_STATUS_INVALID_STATE) == 0);
        CHECK(Diamond(graph, false) == 0);
        strcpy(graph->edges[3].after_id, "missing");
        CHECK(Preserved(graph, plan, UMI_STATUS_INVALID_STATE) == 0);
    } else if (strcmp(mode, "cycle-empty-null") == 0) {
        strcpy(graph->edges[4].before_id, "c-sink");
        strcpy(graph->edges[4].after_id, "z-source");
        graph->edge_count = 5U;
        CHECK(Preserved(graph, plan, UMI_STATUS_INVALID_ARGUMENT) == 0);
        umi_fabric_workflow_graph_init(graph);
        CHECK(Preserved(graph, plan, UMI_STATUS_INVALID_ARGUMENT) == 0);
        CHECK(Preserved(NULL, plan, UMI_STATUS_INVALID_ARGUMENT) == 0);
        CHECK(umi_fabric_workflow_plan_build(graph, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(mode, "allocation") == 0) {
        unsigned char saved[sizeof(*plan)];
        memset(plan, 0xa5, sizeof(*plan));
        memcpy(saved, plan, sizeof(saved));
        fail_allocation = true;
        CHECK(UmiTestPlanBuild(graph, plan) == UMI_STATUS_OUT_OF_MEMORY);
        CHECK(allocations == 1U && live_allocations == 0U);
        CHECK(memcmp(saved, plan, sizeof(saved)) == 0);
        CHECK(UmiTestPlanBuild(graph, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(allocations == 1U);
        fail_allocation = false;
        CHECK(UmiTestPlanBuild(NULL, plan) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(live_allocations == 0U && memcmp(saved, plan, sizeof(saved)) == 0);
        CHECK(UmiTestPlanBuild(graph, plan) == UMI_STATUS_OK);
        CHECK(live_allocations == 0U && plan->count == 5U);
    } else if (strcmp(mode, "duplicate-edges") == 0) {
        for (size_t i = graph->edge_count; i < UMI_FABRIC_MAX_EDGES; ++i)
            CHECK(umi_fabric_workflow_graph_add_edge(graph, "z-source", "a-left") == UMI_STATUS_OK);
        CHECK(umi_fabric_workflow_plan_build(graph, plan) == UMI_STATUS_OK);
        CHECK(plan->count == 5U && strcmp(plan->step_ids[1], "z-source") == 0 &&
              strcmp(plan->step_ids[2], "a-left") == 0);
    } else if (strcmp(mode, "full-chain") == 0) {
        umi_fabric_workflow_graph_init(graph);
        for (size_t i = 0U; i < UMI_FABRIC_MAX_STEPS; ++i) {
            char id[32];
            (void)snprintf(id, sizeof(id), "step-%02zu", UMI_FABRIC_MAX_STEPS-1U-i);
            CHECK(Add(graph, id) == 0);
            if (i != 0U) CHECK(umi_fabric_workflow_graph_add_edge(graph,
                graph->steps[i-1U].step_id, graph->steps[i].step_id) == UMI_STATUS_OK);
        }
        CHECK(umi_fabric_workflow_plan_build(graph, plan) == UMI_STATUS_OK);
        CHECK(plan->count == UMI_FABRIC_MAX_STEPS);
        for (size_t i = 0U; i < plan->count; ++i)
            CHECK(strcmp(plan->step_ids[i], graph->steps[i].step_id) == 0);
    } else if (strcmp(mode, "maximum-id") == 0) {
        umi_fabric_workflow_graph_init(graph);
        char id[UMI_FABRIC_ID_CAPACITY];
        memset(id, 'x', sizeof(id)); id[sizeof(id)-1U] = '\0';
        CHECK(Add(graph, id) == 0);
        CHECK(umi_fabric_workflow_plan_build(graph, plan) == UMI_STATUS_OK);
        CHECK(plan->count == 1U && strcmp(plan->step_ids[0], id) == 0);
    } else if (strcmp(mode, "fingerprint-scope") == 0) {
        CHECK(umi_fabric_workflow_plan_build(graph, plan) == UMI_STATUS_OK);
        uint64_t fingerprint = plan->fingerprint, expected = 0U;
        for (size_t i = 0U; i < plan->count; ++i)
            expected = umi_fabric_hash64(plan->step_ids[i], strlen(plan->step_ids[i]), expected);
        CHECK(expected == fingerprint);
        graph->steps[0].timeout_ms = 999U;
        strcpy(graph->steps[0].operation_id, "different-operation");
        CHECK(umi_fabric_workflow_plan_build(graph, plan) == UMI_STATUS_OK);
        CHECK(plan->fingerprint == fingerprint); /* Historical order-only identity. */
    } else return 2;
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    UmiFabricWorkflowGraph *graph = malloc(sizeof(*graph));
    UmiFabricWorkflowPlan *plan = malloc(sizeof(*plan));
    if (graph == NULL || plan == NULL) { free(graph); free(plan); return 3; }
    int result = Run(argv[1], graph, plan);
    free(plan); free(graph);
    return result;
}
