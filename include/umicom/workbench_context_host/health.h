/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/health.h
 *
 * PURPOSE:
 *   Evaluate context-host readiness and queue pressure for operational diagnostics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_HEALTH_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_HEALTH_H
#include "umicom/workbench_context_host/snapshot.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiWorkbenchContextHostHealthState {
    UMI_WORKBENCH_CONTEXT_HOST_HEALTH_UNKNOWN=0,
    UMI_WORKBENCH_CONTEXT_HOST_HEALTH_HEALTHY=1,
    UMI_WORKBENCH_CONTEXT_HOST_HEALTH_DEGRADED=2,
    UMI_WORKBENCH_CONTEXT_HOST_HEALTH_UNAVAILABLE=3
} UmiWorkbenchContextHostHealthState;
typedef struct UmiWorkbenchContextHostHealth {
    UmiWorkbenchContextHostHealthState state;
    size_t endpoint_count;
    size_t queued_delivery_count;
    uint64_t dropped_delivery_count;
    char message[UMI_WORKBENCH_CONTEXT_HOST_TEXT_CAPACITY];
} UmiWorkbenchContextHostHealth;
UmiStatus umi_workbench_context_host_health_evaluate(
    const UmiWorkbenchContextHost *host,UmiWorkbenchContextHostHealth *out_health);
#ifdef __cplusplus
}
#endif
#endif
