/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_workflow_graph_integrity.c
 *
 * PURPOSE:
 *   Exercise malformed graph storage, refused edits and full-capacity dependency traversal.
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

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)

static int Add(UmiFabricWorkflowGraph *graph, const char *id)
{
    UmiFabricWorkflowStep step;
    CHECK(umi_fabric_workflow_step_init(&step, id, "operation", 100U, false, false) == UMI_STATUS_OK);
    CHECK(umi_fabric_workflow_graph_add_step(graph, &step) == UMI_STATUS_OK);
    return 0;
}
static int Pair(UmiFabricWorkflowGraph *graph)
{
    umi_fabric_workflow_graph_init(graph);
    CHECK(Add(graph, "before") == 0);
    CHECK(Add(graph, "after") == 0);
    return 0;
}
/* Every malformed fixture must be rejected by all consumers before traversal.
 * Whole-object comparison includes unused storage, not only the public counts. */
static int Invalid(UmiFabricWorkflowGraph *graph, UmiFabricWorkflowGraph *saved)
{
    memcpy(saved, graph, sizeof(*saved));
    CHECK(umi_fabric_workflow_graph_validate(graph) == UMI_STATUS_INVALID_STATE);
    for (unsigned initial = 0U; initial < 2U; ++initial) {
        bool cycle = initial != 0U;
        CHECK(umi_fabric_workflow_graph_has_cycle_checked(graph, &cycle) == UMI_STATUS_INVALID_STATE);
        CHECK(cycle == (initial != 0U));
    }
    CHECK(umi_fabric_workflow_graph_has_cycle(graph));
    UmiFabricWorkflowStep step;
    CHECK(umi_fabric_workflow_step_init(&step, "extra", "operation", 1U, false, false) == UMI_STATUS_OK);
    CHECK(umi_fabric_workflow_graph_add_step(graph, &step) == UMI_STATUS_INVALID_STATE);
    CHECK(umi_fabric_workflow_graph_add_edge(graph, "before", "after") == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(saved, graph, sizeof(*saved)) == 0);
    return 0;
}
static int Run(const char *mode, UmiFabricWorkflowGraph *graph, UmiFabricWorkflowGraph *saved)
{
    if (strcmp(mode, "empty-null") == 0) {
        umi_fabric_workflow_graph_init(graph);
        memset(graph->steps, 0xa5, sizeof(graph->steps));
        memset(graph->edges, 0xa5, sizeof(graph->edges));
        bool cycle = true;
        CHECK(umi_fabric_workflow_graph_validate(graph) == UMI_STATUS_OK);
        CHECK(umi_fabric_workflow_graph_has_cycle_checked(graph, &cycle) == UMI_STATUS_OK && !cycle);
        CHECK(!umi_fabric_workflow_graph_has_cycle(NULL));
        CHECK(umi_fabric_workflow_graph_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_fabric_workflow_graph_has_cycle_checked(NULL, &cycle) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!cycle);
        CHECK(umi_fabric_workflow_graph_has_cycle_checked(graph, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_fabric_workflow_graph_add_step(graph, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_fabric_workflow_graph_add_edge(graph, NULL, "after") == UMI_STATUS_INVALID_ARGUMENT);
        return 0;
    }
    CHECK(Pair(graph) == 0);
    if (strcmp(mode, "counts") == 0) {
        const size_t invalid[] = {UMI_FABRIC_MAX_STEPS + 1U, SIZE_MAX};
        for (size_t i = 0U; i < 2U; ++i) {
            graph->step_count = invalid[i];
            CHECK(Invalid(graph, saved) == 0);
            graph->step_count = 2U;
            graph->edge_count = i == 0U ? UMI_FABRIC_MAX_EDGES + 1U : SIZE_MAX;
            CHECK(Invalid(graph, saved) == 0);
            graph->edge_count = 0U;
        }
    } else if (strcmp(mode, "step-text") == 0) {
        for (unsigned field = 0U; field < 2U; ++field) {
            CHECK(Pair(graph) == 0);
            char *text = field == 0U ? graph->steps[1].step_id : graph->steps[1].operation_id;
            memset(text, 'x', UMI_FABRIC_ID_CAPACITY);
            CHECK(Invalid(graph, saved) == 0);
            text[UMI_FABRIC_ID_CAPACITY - 1U] = '\0';
            CHECK(umi_fabric_workflow_graph_validate(graph) == UMI_STATUS_OK);
            text[0] = '\0';
            CHECK(Invalid(graph, saved) == 0);
        }
    } else if (strcmp(mode, "step-domain") == 0) {
        graph->steps[1].timeout_ms = 0U;
        CHECK(Invalid(graph, saved) == 0);
        CHECK(Pair(graph) == 0);
        strcpy(graph->steps[1].step_id, graph->steps[0].step_id);
        CHECK(Invalid(graph, saved) == 0);
    } else if (strcmp(mode, "edge-text") == 0) {
        for (unsigned field = 0U; field < 2U; ++field) {
            CHECK(Pair(graph) == 0);
            CHECK(umi_fabric_workflow_graph_add_edge(graph, "before", "after") == UMI_STATUS_OK);
            char *text = field == 0U ? graph->edges[0].before_id : graph->edges[0].after_id;
            memset(text, 'x', UMI_FABRIC_ID_CAPACITY);
            CHECK(Invalid(graph, saved) == 0);
            text[0] = '\0';
            CHECK(Invalid(graph, saved) == 0);
        }
    } else if (strcmp(mode, "missing-endpoints") == 0) {
        for (unsigned field = 0U; field < 2U; ++field) {
            CHECK(Pair(graph) == 0);
            CHECK(umi_fabric_workflow_graph_add_edge(graph, "before", "after") == UMI_STATUS_OK);
            strcpy(field == 0U ? graph->edges[0].before_id : graph->edges[0].after_id, "missing");
            CHECK(Invalid(graph, saved) == 0);
        }
    } else if (strcmp(mode, "late-invalid") == 0) {
        CHECK(umi_fabric_workflow_graph_add_edge(graph, "before", "after") == UMI_STATUS_OK);
        graph->edges[1] = graph->edges[0];
        graph->edge_count = 2U;
        memset(graph->edges[1].after_id, 'x', UMI_FABRIC_ID_CAPACITY);
        CHECK(Invalid(graph, saved) == 0);
    } else if (strcmp(mode, "cycles") == 0) {
        /* Directly restored cyclic graphs remain structurally inspectable. */
        strcpy(graph->edges[0].before_id, "before");
        strcpy(graph->edges[0].after_id, "before");
        graph->edge_count = 1U;
        bool cycle = false;
        CHECK(umi_fabric_workflow_graph_validate(graph) == UMI_STATUS_OK);
        CHECK(umi_fabric_workflow_graph_has_cycle_checked(graph, &cycle) == UMI_STATUS_OK && cycle);
        strcpy(graph->edges[0].after_id, "after");
        strcpy(graph->edges[1].before_id, "after");
        strcpy(graph->edges[1].after_id, "before");
        graph->edge_count = 2U;
        CHECK(umi_fabric_workflow_graph_has_cycle(graph));
        memcpy(saved, graph, sizeof(*saved));
        CHECK(umi_fabric_workflow_graph_add_edge(graph, "before", "after") == UMI_STATUS_INVALID_STATE);
        CHECK(memcmp(saved, graph, sizeof(*saved)) == 0);
    } else if (strcmp(mode, "refused-edge") == 0) {
        CHECK(umi_fabric_workflow_graph_add_edge(graph, "before", "after") == UMI_STATUS_OK);
        memset(&graph->edges[1], 0xa5, sizeof(graph->edges[1]));
        memcpy(saved, graph, sizeof(*saved));
        CHECK(umi_fabric_workflow_graph_add_edge(graph, "after", "before") == UMI_STATUS_INVALID_STATE);
        CHECK(memcmp(saved, graph, sizeof(*saved)) == 0);
        CHECK(umi_fabric_workflow_graph_add_edge(graph, "before", "before") == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_fabric_workflow_graph_add_edge(graph, "missing", "after") == UMI_STATUS_NOT_FOUND);
        CHECK(memcmp(saved, graph, sizeof(*saved)) == 0);
    } else if (strcmp(mode, "borrowed-input") == 0) {
        CHECK(umi_fabric_workflow_step_init(&graph->steps[2], "extra", "operation", 1U, false, false) == UMI_STATUS_OK);
        CHECK(umi_fabric_workflow_graph_add_step(graph, &graph->steps[2]) == UMI_STATUS_OK);
        strcpy(graph->edges[0].before_id, "before");
        strcpy(graph->edges[0].after_id, "after");
        CHECK(umi_fabric_workflow_graph_add_edge(graph, graph->edges[0].before_id, graph->edges[0].after_id) == UMI_STATUS_OK);
        CHECK(strcmp(graph->steps[2].step_id, "extra") == 0 && graph->edge_count == 1U);
    } else if (strcmp(mode, "duplicate-edge") == 0) {
        for (unsigned i = 0U; i < 2U; ++i)
            CHECK(umi_fabric_workflow_graph_add_edge(graph, "before", "after") == UMI_STATUS_OK);
        CHECK(graph->edge_count == 2U && !umi_fabric_workflow_graph_has_cycle(graph));
    } else if (strcmp(mode, "step-capacity") == 0 || strcmp(mode, "chain-capacity") == 0) {
        umi_fabric_workflow_graph_init(graph);
        for (size_t i = 0U; i < UMI_FABRIC_MAX_STEPS; ++i) {
            char id[32];
            (void)snprintf(id, sizeof(id), "step-%02zu", i);
            CHECK(Add(graph, id) == 0);
            if (i != 0U && strcmp(mode, "chain-capacity") == 0)
                CHECK(umi_fabric_workflow_graph_add_edge(graph, graph->steps[i-1U].step_id, graph->steps[i].step_id) == UMI_STATUS_OK);
        }
        UmiFabricWorkflowStep extra;
        CHECK(umi_fabric_workflow_step_init(&extra, "extra", "operation", 1U, false, false) == UMI_STATUS_OK);
        memcpy(saved, graph, sizeof(*saved));
        CHECK(umi_fabric_workflow_graph_add_step(graph, &extra) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(umi_fabric_workflow_graph_add_step(graph, &graph->steps[0]) == UMI_STATUS_ALREADY_EXISTS);
        CHECK(memcmp(saved, graph, sizeof(*saved)) == 0);
        CHECK(!umi_fabric_workflow_graph_has_cycle(graph));
        if (strcmp(mode, "chain-capacity") == 0) {
            CHECK(umi_fabric_workflow_graph_add_edge(graph, graph->steps[63].step_id, graph->steps[0].step_id) == UMI_STATUS_INVALID_STATE);
            CHECK(memcmp(saved, graph, sizeof(*saved)) == 0);
        }
    } else if (strcmp(mode, "edge-capacity") == 0) {
        for (size_t i = 0U; i < UMI_FABRIC_MAX_EDGES; ++i)
            CHECK(umi_fabric_workflow_graph_add_edge(graph, "before", "after") == UMI_STATUS_OK);
        memcpy(saved, graph, sizeof(*saved));
        CHECK(umi_fabric_workflow_graph_add_edge(graph, "before", "after") == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(saved, graph, sizeof(*saved)) == 0);
    } else return 2;
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    UmiFabricWorkflowGraph *graph = malloc(sizeof(*graph)), *saved = malloc(sizeof(*saved));
    if (graph == NULL || saved == NULL) { free(graph); free(saved); return 3; }
    int result = Run(argv[1], graph, saved);
    free(saved);
    free(graph);
    return result;
}
