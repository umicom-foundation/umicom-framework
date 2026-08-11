/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/suite_runtime.c
 *
 * PURPOSE:
 *   Implement the corresponding public Suite and Inter-Application Runtime contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#include "umicom/integration/suite_runtime.h"

#include <stdio.h>
#include <string.h>

static const UmiIntegrationLaunchItem *find_item(
    const UmiIntegrationSuiteRuntime *runtime,
    const char *application_id)
{
    size_t index;
    for (index = 0U; index < runtime->plan.count; ++index) {
        if (strcmp(runtime->plan.items[index].application_id,
                   application_id) == 0) {
            return &runtime->plan.items[index];
        }
    }
    return NULL;
}

UmiStatus umi_integration_suite_runtime_prepare(
    UmiIntegrationSuiteRuntime *runtime,
    const UmiIntegrationSuiteDefinition *suite,
    const UmiIntegrationRegistry *registry)
{
    UmiStatus status;
    int written;

    if (runtime == NULL || suite == NULL || registry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(runtime, 0, sizeof(*runtime));
    written = snprintf(runtime->suite_id,
                       sizeof(runtime->suite_id),
                       "%s",
                       suite->id);
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

UmiStatus umi_integration_suite_runtime_mark_running(
    UmiIntegrationSuiteRuntime *runtime,
    const char *application_id)
{
    const UmiIntegrationLaunchItem *item;
    if (runtime == NULL || application_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    item = find_item(runtime, application_id);
    if (item == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }

    if (item->kind == UMI_INTEGRATION_DEPENDENCY_REQUIRED) {
        ++runtime->running_required;
    } else {
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

UmiStatus umi_integration_suite_runtime_mark_failed(
    UmiIntegrationSuiteRuntime *runtime,
    const char *application_id)
{
    const UmiIntegrationLaunchItem *item;
    if (runtime == NULL || application_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    item = find_item(runtime, application_id);
    if (item == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }

    if (item->kind == UMI_INTEGRATION_DEPENDENCY_REQUIRED) {
        ++runtime->failed_required;
        runtime->state = UMI_INTEGRATION_SUITE_FAILED;
    } else {
        ++runtime->failed_optional;
        runtime->state = UMI_INTEGRATION_SUITE_DEGRADED;
    }
    return UMI_STATUS_OK;
}

bool umi_integration_suite_runtime_is_usable(
    const UmiIntegrationSuiteRuntime *runtime)
{
    return runtime != NULL &&
           runtime->failed_required == 0U &&
           runtime->plan.missing_required == 0U &&
           runtime->state != UMI_INTEGRATION_SUITE_FAILED;
}
