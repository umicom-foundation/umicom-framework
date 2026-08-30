/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/engine.c
 *
 * PURPOSE:
 *   Implement first-class graph execution while reusing build providers,
 *   process cancellation/timeouts, diagnostic parsing and retained history.
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

struct UmiBuildEngine {
    UmiBuildProfile profile;
    UmiBuildGraph *graph;
    UmiBuildRunner *runner;
    UmiBuildHistory *history;
    UmiCancellationToken *cancellation;
    int owns_cancellation;
    char active_node_id[UMI_BUILD_ID_CAPACITY];
    uint64_t revision;
};

UmiStatus umi_build_engine_create(const UmiBuildEngineConfig *config,
                                  UmiBuildEngine **out_engine)
{
    UmiBuildEngine *engine;
    UmiBuildRunnerConfig runner_config;
    UmiStatus status;
    if (config == NULL || out_engine == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_engine = NULL;
    engine = (UmiBuildEngine *)calloc(1U, sizeof(*engine));
    if (engine == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    engine->profile = config->profile;
    engine->history = config->history;
    engine->cancellation = config->cancellation;
    if (engine->cancellation == NULL) {
        status = umi_cancellation_token_create(&engine->cancellation);
        if (status != UMI_STATUS_OK) {
            umi_build_engine_destroy(engine);
            return status;
        }
        engine->owns_cancellation = 1;
    }
    status = umi_build_graph_create(&engine->graph);
    if (status == UMI_STATUS_OK) {
        memset(&runner_config, 0, sizeof(runner_config));
        runner_config.profile = engine->profile;
        runner_config.history = engine->history;
        runner_config.clock = config->clock;
        runner_config.cancellation = engine->cancellation;
        status = umi_build_runner_create(&runner_config, &engine->runner);
    }
    if (status != UMI_STATUS_OK) {
        umi_build_engine_destroy(engine);
        return status;
    }
    engine->revision = 1U;
    *out_engine = engine;
    return UMI_STATUS_OK;
}

void umi_build_engine_destroy(UmiBuildEngine *engine)
{
    if (engine == NULL) return;
    umi_build_runner_destroy(engine->runner);
    umi_build_graph_destroy(engine->graph);
    if (engine->owns_cancellation)
        umi_cancellation_token_destroy(engine->cancellation);
    free(engine);
}

UmiBuildGraph *umi_build_engine_graph(UmiBuildEngine *engine)
{
    return engine != NULL ? engine->graph : NULL;
}

UmiStatus umi_build_engine_set_profile(UmiBuildEngine *engine,
                                       const UmiBuildProfile *profile)
{
    UmiStatus status;
    if (engine == NULL || profile == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_build_runner_set_profile(engine->runner, profile);
    if (status == UMI_STATUS_OK) {
        engine->profile = *profile;
        engine->revision += 1U;
    }
    return status;
}

UmiStatus umi_build_engine_execute_phase(UmiBuildEngine *engine,
                                         UmiBuildPhase phase,
                                         UmiBuildResult *out_result)
{
    UmiStatus status;
    if (engine == NULL || out_result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_build_runner_run(engine->runner, phase, out_result);
    engine->revision += 1U;
    return status;
}

UmiStatus umi_build_engine_execute_next(UmiBuildEngine *engine,
                                        UmiBuildResult *out_result)
{
    UmiBuildGraphNodeSnapshot node;
    UmiBuildProfile execution_profile;
    UmiStatus status;
    if (engine == NULL || out_result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_build_graph_next_ready(engine->graph, &node);
    if (status != UMI_STATUS_OK) return status;
    memset(out_result, 0, sizeof(*out_result));
    if (node.incremental && node.completed_input_revision != 0U &&
        node.completed_input_revision == node.input_revision) {
        status = umi_build_graph_skip_unchanged(engine->graph, node.node_id);
        if (status == UMI_STATUS_OK) {
            umi_build_result_init(out_result, 0U, node.phase,
                                  engine->profile.profile_id);
            umi_build_result_finish(out_result, UMI_STATUS_OK, 0, 0U);
        }
        return status;
    }
    status = umi_build_graph_start(engine->graph, node.node_id);
    if (status != UMI_STATUS_OK) return status;
    (void)snprintf(engine->active_node_id,
                   sizeof(engine->active_node_id), "%s", node.node_id);
    execution_profile = engine->profile;
    if (node.timeout_ms != 0U) execution_profile.timeout_ms = node.timeout_ms;
    status = umi_build_runner_set_profile(engine->runner, &execution_profile);
    if (status != UMI_STATUS_OK) {
        engine->active_node_id[0] = '\0';
        (void)umi_build_graph_fail(engine->graph, node.node_id, status, -1);
        return status;
    }
    status = umi_build_runner_run(engine->runner, node.phase, out_result);
    (void)umi_build_runner_set_profile(engine->runner, &engine->profile);
    engine->active_node_id[0] = '\0';
    if (status == UMI_STATUS_CANCELLED) {
        (void)umi_build_graph_cancel(engine->graph, node.node_id);
    } else if (status == UMI_STATUS_TIMEOUT) {
        (void)umi_build_graph_timeout(engine->graph, node.node_id);
    } else if (status == UMI_STATUS_OK && out_result->exit_code == 0) {
        (void)umi_build_graph_complete(engine->graph, node.node_id,
                                       out_result->exit_code);
    } else {
        (void)umi_build_graph_fail(engine->graph, node.node_id,
                                   status, out_result->exit_code);
    }
    engine->revision += 1U;
    return status;
}

UmiStatus umi_build_engine_execute_all(UmiBuildEngine *engine,
                                       size_t maximum_nodes,
                                       size_t *out_executed_count)
{
    size_t count = 0U;
    size_t limit = maximum_nodes == 0U
        ? UMI_BUILD_GRAPH_MAX_NODES : maximum_nodes;
    UmiStatus status = UMI_STATUS_OK;
    UmiBuildResult *result = NULL;
    if (engine == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_build_result_create(&result);
    if (status != UMI_STATUS_OK) return status;
    while (count < limit) {
        status = umi_build_engine_execute_next(engine, result);
        if (status == UMI_STATUS_NOT_FOUND) {
            status = UMI_STATUS_OK;
            break;
        }
        count += 1U;
        if (status != UMI_STATUS_OK) break;
        if (result->exit_code != 0) {
            /* Process execution succeeded, but the task itself failed.  Keep
             * the detailed exit code in the result and stop the graph. */
            status = UMI_STATUS_INTERNAL_ERROR;
            break;
        }
    }
    umi_build_result_destroy(result);
    if (out_executed_count != NULL) *out_executed_count = count;
    return status;
}

void umi_build_engine_request_cancel(UmiBuildEngine *engine)
{
    if (engine != NULL) {
        umi_cancellation_token_request(engine->cancellation);
        engine->revision += 1U;
    }
}

void umi_build_engine_reset_cancel(UmiBuildEngine *engine)
{
    if (engine != NULL) {
        umi_cancellation_token_reset(engine->cancellation);
        engine->revision += 1U;
    }
}

UmiStatus umi_build_engine_retry(UmiBuildEngine *engine,
                                 const char *node_id)
{
    if (engine == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_build_engine_reset_cancel(engine);
    return umi_build_graph_retry(engine->graph, node_id);
}

UmiStatus umi_build_engine_invalidate(UmiBuildEngine *engine,
                                      const char *node_id,
                                      uint64_t input_revision)
{
    if (engine == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_build_graph_invalidate(engine->graph, node_id, input_revision);
}

UmiStatus umi_build_engine_snapshot(UmiBuildEngine *engine,
                                    UmiBuildEngineSnapshot *out_snapshot)
{
    UmiStatus status;
    if (engine == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->structure_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_BUILD_ENGINE_API_VERSION;
    status = umi_build_graph_snapshot(engine->graph, &out_snapshot->graph);
    if (status != UMI_STATUS_OK) return status;
    out_snapshot->result_count = umi_build_history_count(engine->history);
    out_snapshot->next_operation_id =
        umi_build_runner_next_operation_id(engine->runner);
    (void)snprintf(out_snapshot->active_node_id,
                   sizeof(out_snapshot->active_node_id), "%s",
                   engine->active_node_id);
    out_snapshot->cancellation_requested =
        umi_cancellation_token_is_requested(engine->cancellation);
    out_snapshot->revision = engine->revision;
    return UMI_STATUS_OK;
}
