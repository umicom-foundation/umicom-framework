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

/*
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#ifndef UMICOM_INTEGRATION_SUITE_RUNTIME_H
#define UMICOM_INTEGRATION_SUITE_RUNTIME_H

#include "umicom/base/status.h"
#include "umicom/integration/launch_plan.h"

/**
 * List the named integration suite state values accepted by this public contract.
 */
typedef enum UmiIntegrationSuiteState {
    UMI_INTEGRATION_SUITE_IDLE = 0,
    UMI_INTEGRATION_SUITE_PREPARED,
    UMI_INTEGRATION_SUITE_STARTING,
    UMI_INTEGRATION_SUITE_RUNNING,
    UMI_INTEGRATION_SUITE_DEGRADED,
    UMI_INTEGRATION_SUITE_FAILED,
    UMI_INTEGRATION_SUITE_STOPPED
} UmiIntegrationSuiteState;

/**
 * Represent the integration suite runtime data shared with callers of this public
 * contract.
 */
typedef struct UmiIntegrationSuiteRuntime {
    char suite_id[UMI_INTEGRATION_ID_CAPACITY];
    UmiIntegrationSuiteState state;
    UmiIntegrationLaunchPlan plan;
    size_t running_required;
    size_t running_optional;
    size_t failed_required;
    size_t failed_optional;
} UmiIntegrationSuiteRuntime;

/**
 * Provide the integration suite runtime prepare operation used by this module and its
 * client applications.
 */
/* Preparation is evidence only. An already-running member in the registry is
 * counted once; a ready-to-launch member is not counted as running. Incomplete
 * required membership leaves the suite PREPARED or STARTING, never RUNNING. */
UmiStatus umi_integration_suite_runtime_prepare(
    UmiIntegrationSuiteRuntime *runtime,
    const UmiIntegrationSuiteDefinition *suite,
    const UmiIntegrationRegistry *registry);
/**
 * Provide the integration suite runtime mark running operation used by this module and its
 * client applications.
 */
/* These functions take observed lifecycle evidence from the owning supervisor.
 * They never create/stop a process and never contact sibling products.
 * Duplicate notifications return INVALID_STATE; this prevents counted duplicates
 * from producing a false RUNNING status. An explicit RUNNING observation can
 * recover a previously FAILED member. Failed/missing/disabled startup attempts
 * are not silently retried by the Framework. */
UmiStatus umi_integration_suite_runtime_mark_running(
    UmiIntegrationSuiteRuntime *runtime,
    const char *application_id);
/**
 * Provide the integration suite runtime mark failed operation used by this module and its
 * client applications.
 */
UmiStatus umi_integration_suite_runtime_mark_failed(
    UmiIntegrationSuiteRuntime *runtime,
    const char *application_id);
/**
 * Provide the integration suite runtime is usable operation used by this module and its
 * client applications.
 */
/* Usable means every required member has RUNNING evidence and at least one
 * member is running. Missing/failed required members are always fail-closed;
 * an absent/failed optional member may yield DEGRADED. STARTING/IDLE/PREPARED
 * are never a claim that a runnable application suite exists. */
bool umi_integration_suite_runtime_is_usable(
    const UmiIntegrationSuiteRuntime *runtime);

#endif
