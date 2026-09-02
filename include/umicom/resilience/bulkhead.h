/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/resilience/bulkhead.h
 *
 * PURPOSE:
 *   Limit concurrent work and bounded waiting capacity for isolated providers,
 *   plug-ins, engines and worker pools.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RESILIENCE_BULKHEAD_H
#define UMICOM_RESILIENCE_BULKHEAD_H

#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the bulkhead config data shared with callers of this public contract.
 */
typedef struct UmiBulkheadConfig { size_t maximum_concurrent; size_t maximum_waiting; } UmiBulkheadConfig;
/**
 * Represent the bulkhead snapshot data shared with callers of this public contract.
 */
typedef struct UmiBulkheadSnapshot { size_t active; size_t waiting; size_t rejected; } UmiBulkheadSnapshot;
/**
 * Represent the bulkhead data shared with callers of this public contract.
 */
typedef struct UmiBulkhead UmiBulkhead;
/**
 * Initialise bulkhead from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_bulkhead_create(const UmiBulkheadConfig *config, UmiBulkhead **out_bulkhead);
/**
 * Release or reset state held by bulkhead so the same storage can be reused safely.
 */
void umi_bulkhead_destroy(UmiBulkhead *bulkhead);
/**
 * Provide the bulkhead enter operation used by this module and its client applications.
 */
UmiStatus umi_bulkhead_enter(UmiBulkhead *bulkhead, int allow_waiting);
/**
 * Provide the bulkhead leave operation used by this module and its client applications.
 */
void umi_bulkhead_leave(UmiBulkhead *bulkhead);
/**
 * Provide the bulkhead waiting done operation used by this module and its client
 * applications.
 */
void umi_bulkhead_waiting_done(UmiBulkhead *bulkhead);
/**
 * Provide the bulkhead snapshot operation used by this module and its client applications.
 */
UmiBulkheadSnapshot umi_bulkhead_snapshot(UmiBulkhead *bulkhead);
#ifdef __cplusplus
}
#endif

#endif
