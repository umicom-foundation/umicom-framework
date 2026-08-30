/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/health.h
 *
 * PURPOSE:
 *   Evaluate whether live workbench interaction sources are registered, flowing and free from excessive rejection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_HEALTH_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_HEALTH_H
#include "umicom/workbench_context_source/snapshot.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiWorkbenchContextSourceHealthState {
    UMI_WORKBENCH_CONTEXT_SOURCE_HEALTH_UNKNOWN = 0,
    UMI_WORKBENCH_CONTEXT_SOURCE_HEALTH_HEALTHY = 1,
    UMI_WORKBENCH_CONTEXT_SOURCE_HEALTH_DEGRADED = 2,
    UMI_WORKBENCH_CONTEXT_SOURCE_HEALTH_UNAVAILABLE = 3
} UmiWorkbenchContextSourceHealthState;

typedef struct UmiWorkbenchContextSourceHealth {
    UmiWorkbenchContextSourceHealthState state;
    size_t registered_sources;
    size_t active_sources;
    uint64_t accepted_samples;
    uint64_t rejected_samples;
    uint64_t duplicate_samples;
    uint64_t throttled_samples;
    char message[UMI_WORKBENCH_CONTEXT_SOURCE_TEXT_CAPACITY];
} UmiWorkbenchContextSourceHealth;

UmiStatus umi_workbench_context_source_health_evaluate(
    const UmiWorkbenchContextSourceService *service,
    UmiWorkbenchContextSourceHealth *out_health);

#ifdef __cplusplus
}
#endif
#endif
