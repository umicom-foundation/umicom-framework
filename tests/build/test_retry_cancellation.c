/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/build/test_retry_cancellation.c
 *
 * PURPOSE:
 *   Verify rejected graph retries preserve cancellation and accepted retries reset it through public owners.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/build/engine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); exit(1); \
} } while (0)

static void AddNode(UmiBuildGraph *graph, const char *id, uint32_t attempts)
{
    UmiBuildGraphNodeSnapshot node;
    umi_build_graph_node_init(&node, id, id, UMI_BUILD_PHASE_BUILD);
    node.maximum_attempts = attempts;
    CHECK(umi_build_graph_add_node(graph, &node) == UMI_STATUS_OK);
}
static void Capture(UmiBuildEngine *engine, UmiBuildEngineSnapshot *snapshot,
    UmiBuildGraphNodeSnapshot *node)
{
    CHECK(umi_build_engine_snapshot(engine, snapshot) == UMI_STATUS_OK);
    CHECK(umi_build_graph_find(umi_build_engine_graph(engine), "work", node) == UMI_STATUS_OK);
}
int main(int argc, char **argv)
{
    if (argc != 4) return 2;
    const char *owner = argv[1], *cancellation = argv[2], *name = argv[3];
    if ((strcmp(owner, "owned") != 0 && strcmp(owner, "borrowed") != 0) ||
        (strcmp(cancellation, "requested") != 0 && strcmp(cancellation, "clear") != 0)) return 2;
    static const char *const known[] = {"null-engine", "null-node", "missing", "ready",
        "waiting", "blocked", "running", "succeeded", "attempts", "failed",
        "cancelled", "timed-out", "repeat"};
    int recognised = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(name, known[i]) == 0) recognised = 1;
    if (!recognised) return 2;

    UmiBuildEngineConfig config = {0};
    UmiBuildEngine *engine = NULL;
    UmiCancellationToken *token = NULL;
    UmiBuildHistory *history = NULL;
    UmiBuildEngineSnapshot before, after;
    UmiBuildGraphNodeSnapshot before_node, after_node, other_before, other_after;
    umi_build_profile_init(&config.profile);
    CHECK(umi_build_history_create(2U, &history) == UMI_STATUS_OK);
    config.history = history;
    if (strcmp(owner, "borrowed") == 0) {
        CHECK(umi_cancellation_token_create(&token) == UMI_STATUS_OK);
        config.cancellation = token;
    }
    CHECK(umi_build_engine_create(&config, &engine) == UMI_STATUS_OK);
    UmiBuildGraph *graph = umi_build_engine_graph(engine);
    AddNode(graph, "other", 3U);
    AddNode(graph, "work", strcmp(name, "attempts") == 0 ? 1U : 3U);
    if (strcmp(name, "waiting") == 0 || strcmp(name, "blocked") == 0) {
        CHECK(umi_build_graph_add_dependency(graph, "work", "other") == UMI_STATUS_OK);
        if (strcmp(name, "blocked") == 0) {
            CHECK(umi_build_graph_start(graph, "other") == UMI_STATUS_OK);
            CHECK(umi_build_graph_fail(graph, "other", UMI_STATUS_IO_ERROR, 4) == UMI_STATUS_OK);
        }
    } else if (strcmp(name, "running") == 0 || strcmp(name, "succeeded") == 0 ||
        strcmp(name, "attempts") == 0 || strcmp(name, "failed") == 0 ||
        strcmp(name, "cancelled") == 0 || strcmp(name, "timed-out") == 0 ||
        strcmp(name, "repeat") == 0) {
        /* Graph start changes model state only. No engine execute function is
         * called by this fixture, so no provider or child process is involved. */
        CHECK(umi_build_graph_start(graph, "work") == UMI_STATUS_OK);
        CHECK(umi_build_graph_set_progress(graph, "work", 2500U) == UMI_STATUS_OK);
        if (strcmp(name, "succeeded") == 0)
            CHECK(umi_build_graph_complete(graph, "work", 0) == UMI_STATUS_OK);
        else if (strcmp(name, "cancelled") == 0)
            CHECK(umi_build_graph_cancel(graph, "work") == UMI_STATUS_OK);
        else if (strcmp(name, "timed-out") == 0)
            CHECK(umi_build_graph_timeout(graph, "work") == UMI_STATUS_OK);
        else if (strcmp(name, "running") != 0)
            CHECK(umi_build_graph_fail(graph, "work", UMI_STATUS_IO_ERROR, 9) == UMI_STATUS_OK);
    }
    if (strcmp(name, "repeat") == 0)
        CHECK(umi_build_engine_retry(engine, "work") == UMI_STATUS_OK);
    if (strcmp(cancellation, "requested") == 0) umi_build_engine_request_cancel(engine);
    Capture(engine, &before, &before_node);
    CHECK(umi_build_graph_find(graph, "other", &other_before) == UMI_STATUS_OK);
    uint64_t generation = token != NULL ? umi_cancellation_token_generation(token) : 0U;
    const char *id = strcmp(name, "null-node") == 0 ? NULL :
        strcmp(name, "missing") == 0 ? "absent" : "work";
    UmiStatus expected = strcmp(name, "null-engine") == 0 || strcmp(name, "null-node") == 0
        ? UMI_STATUS_INVALID_ARGUMENT : strcmp(name, "missing") == 0 ? UMI_STATUS_NOT_FOUND :
        strcmp(name, "failed") == 0 || strcmp(name, "cancelled") == 0 || strcmp(name, "timed-out") == 0
        ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
    CHECK(umi_build_engine_retry(strcmp(name, "null-engine") == 0 ? NULL : engine, id) == expected);
    Capture(engine, &after, &after_node);
    CHECK(umi_build_graph_find(graph, "other", &other_after) == UMI_STATUS_OK);
    CHECK(memcmp(&other_before, &other_after, sizeof(other_before)) == 0);
    CHECK(umi_build_history_count(history) == 0U && after.next_operation_id == before.next_operation_id);
    if (expected != UMI_STATUS_OK) {
        CHECK(memcmp(&before, &after, sizeof(before)) == 0);
        CHECK(memcmp(&before_node, &after_node, sizeof(before_node)) == 0);
        if (token != NULL) CHECK(umi_cancellation_token_generation(token) == generation);
    } else {
        CHECK(after.cancellation_requested == 0);
        CHECK(after.revision == before.revision + 1U);
        CHECK(after_node.state == UMI_BUILD_NODE_READY);
        CHECK(after_node.attempt_count == before_node.attempt_count);
        CHECK(after_node.maximum_attempts == before_node.maximum_attempts);
        CHECK(after_node.progress_basis_points == 0U);
        CHECK(after_node.last_status == UMI_STATUS_OK && after_node.last_exit_code == 0);
        CHECK(after.graph.running_count == 0U && after.result_count == 0U);
        if (token != NULL) CHECK(!umi_cancellation_token_is_requested(token));
    }
    umi_build_engine_destroy(engine);
    /* A borrowed token remains caller-owned after the engine is destroyed. */
    if (token != NULL) {
        umi_cancellation_token_request(token);
        CHECK(umi_cancellation_token_is_requested(token));
        umi_cancellation_token_destroy(token);
    }
    umi_build_history_destroy(history);
    return 0;
}
