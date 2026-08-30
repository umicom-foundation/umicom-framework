/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/engine.h
 *
 * PURPOSE:
 *   Execute ready graph nodes through the existing provider-neutral runner,
 *   retain results and expose cancellation, retry and incremental controls.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BUILD_ENGINE_H
#define UMICOM_BUILD_ENGINE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/build/graph.h"
#include "umicom/build/runner.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_BUILD_ENGINE_API_VERSION 2U

typedef struct UmiBuildEngineConfig {
    UmiBuildProfile profile;
    UmiBuildHistory *history;
    UmiClock *clock;
    UmiCancellationToken *cancellation;
} UmiBuildEngineConfig;

typedef struct UmiBuildEngineSnapshot {
    uint32_t structure_size;
    uint32_t api_version;
    UmiBuildGraphSnapshot graph;
    size_t result_count;
    uint64_t next_operation_id;
    char active_node_id[UMI_BUILD_ID_CAPACITY];
    int cancellation_requested;
    uint64_t revision;
} UmiBuildEngineSnapshot;

typedef struct UmiBuildEngine UmiBuildEngine;

UmiStatus umi_build_engine_create(const UmiBuildEngineConfig *config,
                                  UmiBuildEngine **out_engine);
void umi_build_engine_destroy(UmiBuildEngine *engine);
UmiBuildGraph *umi_build_engine_graph(UmiBuildEngine *engine);
UmiStatus umi_build_engine_set_profile(UmiBuildEngine *engine,
                                       const UmiBuildProfile *profile);
UmiStatus umi_build_engine_execute_phase(UmiBuildEngine *engine,
                                         UmiBuildPhase phase,
                                         UmiBuildResult *out_result);
UmiStatus umi_build_engine_execute_next(UmiBuildEngine *engine,
                                        UmiBuildResult *out_result);
UmiStatus umi_build_engine_execute_all(UmiBuildEngine *engine,
                                       size_t maximum_nodes,
                                       size_t *out_executed_count);
void umi_build_engine_request_cancel(UmiBuildEngine *engine);
void umi_build_engine_reset_cancel(UmiBuildEngine *engine);
UmiStatus umi_build_engine_retry(UmiBuildEngine *engine,
                                 const char *node_id);
UmiStatus umi_build_engine_invalidate(UmiBuildEngine *engine,
                                      const char *node_id,
                                      uint64_t input_revision);
UmiStatus umi_build_engine_snapshot(UmiBuildEngine *engine,
                                    UmiBuildEngineSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif
#endif
