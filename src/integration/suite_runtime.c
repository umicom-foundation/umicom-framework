/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/suite_runtime.c
 *
 * PURPOSE:
 *   Implement the corresponding public Suite and Inter-Application Runtime contract.
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

#include "umicom/integration/suite_runtime.h"

#include <stdio.h>
#include <string.h>

/* Provide the find item operation used by this module and its client applications. */
static const UmiIntegrationLaunchItem *find_item(
    const UmiIntegrationSuiteRuntime *runtime,
    const char *application_id)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < runtime->plan.count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(runtime->plan.items[index].application_id,
                   application_id) == 0) {
            return &runtime->plan.items[index];
        }
    }
    return NULL;
}

/*
 * Provide the integration suite runtime prepare operation used by this module and its
 * client applications.
 */
UmiStatus umi_integration_suite_runtime_prepare(
    UmiIntegrationSuiteRuntime *runtime,
    const UmiIntegrationSuiteDefinition *suite,
    const UmiIntegrationRegistry *registry)
{
    UmiStatus status;
    int written;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || suite == NULL || registry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(runtime, 0, sizeof(*runtime));
    written = snprintf(runtime->suite_id,
                       sizeof(runtime->suite_id),
                       "%s",
                       suite->id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(runtime->suite_id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    status = umi_integration_launch_plan_build(
        suite, registry, &runtime->plan);
    runtime->state = status == UMI_STATUS_OK
        ? UMI_INTEGRATION_SUITE_PREPARED
        : UMI_INTEGRATION_SUITE_FAILED;
    return status;
}

/*
 * Provide the integration suite runtime mark running operation used by this module and its
 * client applications.
 */
UmiStatus umi_integration_suite_runtime_mark_running(
    UmiIntegrationSuiteRuntime *runtime,
    const char *application_id)
{
    const UmiIntegrationLaunchItem *item;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || application_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    item = find_item(runtime, application_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (item->kind == UMI_INTEGRATION_DEPENDENCY_REQUIRED) {
        ++runtime->running_required;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        ++runtime->running_optional;
    }

    runtime->state = runtime->failed_required > 0U
        ? UMI_INTEGRATION_SUITE_FAILED
        : (runtime->failed_optional > 0U ||
           runtime->plan.missing_optional > 0U)
          ? UMI_INTEGRATION_SUITE_DEGRADED
          : UMI_INTEGRATION_SUITE_RUNNING;
    return UMI_STATUS_OK;
}

/*
 * Provide the integration suite runtime mark failed operation used by this module and its
 * client applications.
 */
UmiStatus umi_integration_suite_runtime_mark_failed(
    UmiIntegrationSuiteRuntime *runtime,
    const char *application_id)
{
    const UmiIntegrationLaunchItem *item;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || application_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    item = find_item(runtime, application_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (item->kind == UMI_INTEGRATION_DEPENDENCY_REQUIRED) {
        ++runtime->failed_required;
        runtime->state = UMI_INTEGRATION_SUITE_FAILED;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        ++runtime->failed_optional;
        runtime->state = UMI_INTEGRATION_SUITE_DEGRADED;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the integration suite runtime is usable operation used by this module and its
 * client applications.
 */
bool umi_integration_suite_runtime_is_usable(
    const UmiIntegrationSuiteRuntime *runtime)
{
    return runtime != NULL &&
           runtime->failed_required == 0U &&
           runtime->plan.missing_required == 0U &&
           runtime->state != UMI_INTEGRATION_SUITE_FAILED;
}
