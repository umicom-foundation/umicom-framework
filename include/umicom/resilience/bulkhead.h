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
typedef struct UmiBulkheadConfig { size_t maximum_concurrent; size_t maximum_waiting; } UmiBulkheadConfig;
typedef struct UmiBulkheadSnapshot { size_t active; size_t waiting; size_t rejected; } UmiBulkheadSnapshot;
typedef struct UmiBulkhead UmiBulkhead;
UmiStatus umi_bulkhead_create(const UmiBulkheadConfig *config, UmiBulkhead **out_bulkhead);
void umi_bulkhead_destroy(UmiBulkhead *bulkhead);
UmiStatus umi_bulkhead_enter(UmiBulkhead *bulkhead, int allow_waiting);
void umi_bulkhead_leave(UmiBulkhead *bulkhead);
void umi_bulkhead_waiting_done(UmiBulkhead *bulkhead);
UmiBulkheadSnapshot umi_bulkhead_snapshot(UmiBulkhead *bulkhead);
#ifdef __cplusplus
}
#endif

#endif
