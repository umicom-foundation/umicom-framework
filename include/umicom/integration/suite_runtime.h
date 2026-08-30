/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/suite_runtime.h
 *
 * PURPOSE:
 *   Track suite readiness and member lifecycle independently from process-launch implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#ifndef UMICOM_INTEGRATION_SUITE_RUNTIME_H
#define UMICOM_INTEGRATION_SUITE_RUNTIME_H

#include "umicom/base/status.h"
#include "umicom/integration/launch_plan.h"

typedef enum UmiIntegrationSuiteState {
    UMI_INTEGRATION_SUITE_IDLE = 0,
    UMI_INTEGRATION_SUITE_PREPARED,
    UMI_INTEGRATION_SUITE_STARTING,
    UMI_INTEGRATION_SUITE_RUNNING,
    UMI_INTEGRATION_SUITE_DEGRADED,
    UMI_INTEGRATION_SUITE_FAILED,
    UMI_INTEGRATION_SUITE_STOPPED
} UmiIntegrationSuiteState;

typedef struct UmiIntegrationSuiteRuntime {
    char suite_id[UMI_INTEGRATION_ID_CAPACITY];
    UmiIntegrationSuiteState state;
    UmiIntegrationLaunchPlan plan;
    size_t running_required;
    size_t running_optional;
    size_t failed_required;
    size_t failed_optional;
} UmiIntegrationSuiteRuntime;

UmiStatus umi_integration_suite_runtime_prepare(
    UmiIntegrationSuiteRuntime *runtime,
    const UmiIntegrationSuiteDefinition *suite,
    const UmiIntegrationRegistry *registry);
UmiStatus umi_integration_suite_runtime_mark_running(
    UmiIntegrationSuiteRuntime *runtime,
    const char *application_id);
UmiStatus umi_integration_suite_runtime_mark_failed(
    UmiIntegrationSuiteRuntime *runtime,
    const char *application_id);
bool umi_integration_suite_runtime_is_usable(
    const UmiIntegrationSuiteRuntime *runtime);

#endif
