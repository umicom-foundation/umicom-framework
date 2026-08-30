/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/health.h
 *
 * PURPOSE:
 *   Evaluate provider readiness, publication pressure and selection conversion health.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_HEALTH_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_HEALTH_H
#include "umicom/workbench_selection_provider/snapshot.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiWorkbenchSelectionProviderHealthState {
    UMI_WORKBENCH_SELECTION_PROVIDER_HEALTH_UNKNOWN = 0,
    UMI_WORKBENCH_SELECTION_PROVIDER_HEALTH_HEALTHY = 1,
    UMI_WORKBENCH_SELECTION_PROVIDER_HEALTH_DEGRADED = 2,
    UMI_WORKBENCH_SELECTION_PROVIDER_HEALTH_UNAVAILABLE = 3
} UmiWorkbenchSelectionProviderHealthState;

typedef struct UmiWorkbenchSelectionProviderHealth {
    UmiWorkbenchSelectionProviderHealthState state;
    size_t provider_count;
    uint64_t published_count;
    uint64_t rejection_count;
    uint64_t error_count;
    char message[UMI_WORKBENCH_SELECTION_PROVIDER_TEXT_CAPACITY];
} UmiWorkbenchSelectionProviderHealth;

UmiStatus umi_workbench_selection_provider_health_evaluate(
    const UmiWorkbenchSelectionProviderService *service,
    UmiWorkbenchSelectionProviderHealth *out_health);

#ifdef __cplusplus
}
#endif
#endif
