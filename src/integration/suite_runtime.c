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

/*
 * SOURCE PRESERVATION: The original counter-only lifecycle is retained here
 * for engineering review. It accepted duplicate running/failed notices, which
 * inflated counters and could mark a suite running after only one required
 * member had started. The replacement below uses the EXISTING launch-plan
 * item's disposition as per-member evidence in this runtime's PRIVATE COPY.
 * Public structure sizes and the original C entry points are unchanged.
 */
#if 0
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
#endif

/* This mutable lookup operates on the already-copied plan, never on the
 * application's registry or another product's state. */
static UmiIntegrationLaunchItem *FindRuntimeItem(UmiIntegrationSuiteRuntime *runtime,
                                                const char *application_id)
{
    size_t index;
    if (runtime->plan.count > UMI_INTEGRATION_MAX_MEMBERS)
        return NULL;
    for (index = 0U; index < runtime->plan.count; ++index) {
        if (memchr(runtime->plan.items[index].application_id, '\0',
                   sizeof(runtime->plan.items[index].application_id)) == NULL)
            return NULL;
        if (strcmp(runtime->plan.items[index].application_id, application_id) == 0)
            return &runtime->plan.items[index];
    }
    return NULL;
}

/* Recompute ALL counts from unique per-member observations. This prevents
 * overcounting after duplicate callbacks or a FAILED -> RUNNING recovery.
 * Only the runtime COPY changes; a separately built launch plan remains a
 * description of availability and is not rewritten during process activity. */
static void RecomputeEvidence(UmiIntegrationSuiteRuntime *runtime)
{
    size_t index;
    size_t required_total = 0U;
    size_t starting_total = 0U;
    size_t stopped_total = 0U;

    runtime->running_required = 0U;
    runtime->running_optional = 0U;
    runtime->failed_required = 0U;
    runtime->failed_optional = 0U;
    for (index = 0U; index < runtime->plan.count; ++index) {
        const UmiIntegrationLaunchItem *item = &runtime->plan.items[index];
        const bool required = item->kind == UMI_INTEGRATION_DEPENDENCY_REQUIRED;
        const bool running = item->disposition == UMI_INTEGRATION_LAUNCH_ALREADY_RUNNING ||
                             item->disposition == UMI_INTEGRATION_LAUNCH_OBSERVED_RUNNING;
        const bool failed = item->disposition == UMI_INTEGRATION_LAUNCH_OBSERVED_FAILED;
        if (item->disposition == UMI_INTEGRATION_LAUNCH_OBSERVED_STARTING)
            ++starting_total;
        if (item->disposition == UMI_INTEGRATION_LAUNCH_OBSERVED_STOPPED)
            ++stopped_total;
        if (required)
            ++required_total;
        if (running) {
            if (required) ++runtime->running_required;
            else ++runtime->running_optional;
        }
        if (failed) {
            if (required) ++runtime->failed_required;
            else ++runtime->failed_optional;
        }
    }

    if (runtime->plan.missing_required > 0U || runtime->failed_required > 0U) {
        runtime->state = UMI_INTEGRATION_SUITE_FAILED;
    } else if (runtime->running_required == required_total &&
               (runtime->running_required + runtime->running_optional) > 0U) {
        runtime->state = (runtime->plan.missing_optional > 0U ||
                          runtime->failed_optional > 0U)
            ? UMI_INTEGRATION_SUITE_DEGRADED
            : UMI_INTEGRATION_SUITE_RUNNING;
    } else if (runtime->running_required + runtime->running_optional +
               starting_total > 0U) {
        /* One member being alive or STARTING is not suite-wide readiness
         * while any required member lacks RUNNING evidence. */
        runtime->state = UMI_INTEGRATION_SUITE_STARTING;
    } else if (stopped_total > 0U) {
        /* Completed clean exits outrank historical optional failures when
         * there are no observed live/starting suite processes. */
        runtime->state = UMI_INTEGRATION_SUITE_STOPPED;
    } else if (runtime->failed_optional > 0U) {
        /* A failed optional launch without a required ready member is not a
         * running or healthy suite. Preserve the failed member's evidence. */
        runtime->state = UMI_INTEGRATION_SUITE_STARTING;
    } else {
        runtime->state = UMI_INTEGRATION_SUITE_PREPARED;
    }
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
    UmiIntegrationSuiteRuntime candidate = {0};
    UmiStatus status;
    size_t length;
    if (runtime == NULL || suite == NULL || registry == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* The plan builder validates this same fixed-length suite ID. Copy only
     * after the required terminator has been proved to be inside the field. */
    if (memchr(suite->id, '\0', sizeof(suite->id)) == NULL)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    length = strlen(suite->id);
    if (length == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memcpy(candidate.suite_id, suite->id, length + 1U);

    status = umi_integration_launch_plan_build(suite, registry, &candidate.plan);
    if (status != UMI_STATUS_OK && status != UMI_STATUS_UNAVAILABLE)
        return status; /* Malformed source must not publish a half plan. */
    RecomputeEvidence(&candidate);
    *runtime = candidate;
    return status; /* UNAVAILABLE retains the complete blocked plan. */
}

/* Caller-provided IDs may be short string literals. Never read a fixed
 * 128-byte block past their terminator merely to search for NUL. */
static bool ValidInputId(const char *text)
{
    size_t index;
    if (text == NULL || text[0] == '\0')
        return false;
    for (index = 0U; index < UMI_INTEGRATION_ID_CAPACITY; ++index)
        if (text[index] == '\0')
            return true;
    return false;
}

/*
 * Provide the integration suite runtime mark running operation used by this module and its
 * client applications.
 */
UmiStatus umi_integration_suite_runtime_mark_running(
    UmiIntegrationSuiteRuntime *runtime,
    const char *application_id)
{
    UmiIntegrationLaunchItem *item;
    if (runtime == NULL || !ValidInputId(application_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (runtime->plan.count > UMI_INTEGRATION_MAX_MEMBERS)
        return UMI_STATUS_INVALID_ARGUMENT;
    item = FindRuntimeItem(runtime, application_id);
    if (item == NULL)
        return UMI_STATUS_NOT_FOUND;
    if (item->disposition != UMI_INTEGRATION_LAUNCH_READY &&
        item->disposition != UMI_INTEGRATION_LAUNCH_OBSERVED_STARTING &&
        item->disposition != UMI_INTEGRATION_LAUNCH_OBSERVED_STOPPED &&
        item->disposition != UMI_INTEGRATION_LAUNCH_OBSERVED_FAILED)
        return UMI_STATUS_INVALID_STATE;
    item->disposition = UMI_INTEGRATION_LAUNCH_OBSERVED_RUNNING;
    RecomputeEvidence(runtime);
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
    UmiIntegrationLaunchItem *item;
    if (runtime == NULL || !ValidInputId(application_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (runtime->plan.count > UMI_INTEGRATION_MAX_MEMBERS)
        return UMI_STATUS_INVALID_ARGUMENT;
    item = FindRuntimeItem(runtime, application_id);
    if (item == NULL)
        return UMI_STATUS_NOT_FOUND;
    if (item->disposition != UMI_INTEGRATION_LAUNCH_READY &&
        item->disposition != UMI_INTEGRATION_LAUNCH_OBSERVED_STARTING &&
        item->disposition != UMI_INTEGRATION_LAUNCH_ALREADY_RUNNING &&
        item->disposition != UMI_INTEGRATION_LAUNCH_OBSERVED_RUNNING)
        return UMI_STATUS_INVALID_STATE;
    item->disposition = UMI_INTEGRATION_LAUNCH_OBSERVED_FAILED;
    RecomputeEvidence(runtime);
    return UMI_STATUS_OK;
}

/* R02: An actual process start is distinct from a launchable executable.
 * The integration runtime records supervisor evidence but does not create
 * another process supervisor or make a process readiness handshake claim. */
UmiStatus umi_integration_suite_runtime_mark_starting(
    UmiIntegrationSuiteRuntime *runtime,
    const char *application_id)
{
    UmiIntegrationLaunchItem *item;
    if (runtime == NULL || !ValidInputId(application_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (runtime->plan.count > UMI_INTEGRATION_MAX_MEMBERS)
        return UMI_STATUS_INVALID_ARGUMENT;
    item = FindRuntimeItem(runtime, application_id);
    if (item == NULL)
        return UMI_STATUS_NOT_FOUND;
    if (item->disposition != UMI_INTEGRATION_LAUNCH_READY &&
        item->disposition != UMI_INTEGRATION_LAUNCH_OBSERVED_STOPPED &&
        item->disposition != UMI_INTEGRATION_LAUNCH_OBSERVED_FAILED)
        return UMI_STATUS_INVALID_STATE;
    item->disposition = UMI_INTEGRATION_LAUNCH_OBSERVED_STARTING;
    RecomputeEvidence(runtime);
    return UMI_STATUS_OK;
}

/* R02: The caller must first match the authoritative child/process token.
 * This function only changes the existing suite runtime's copied evidence.
 * A disappeared executable is NOT a confirmed process exit. */
UmiStatus umi_integration_suite_runtime_mark_stopped(
    UmiIntegrationSuiteRuntime *runtime,
    const char *application_id)
{
    UmiIntegrationLaunchItem *item;
    if (runtime == NULL || !ValidInputId(application_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (runtime->plan.count > UMI_INTEGRATION_MAX_MEMBERS)
        return UMI_STATUS_INVALID_ARGUMENT;
    item = FindRuntimeItem(runtime, application_id);
    if (item == NULL)
        return UMI_STATUS_NOT_FOUND;
    if (item->disposition != UMI_INTEGRATION_LAUNCH_ALREADY_RUNNING &&
        item->disposition != UMI_INTEGRATION_LAUNCH_OBSERVED_RUNNING &&
        item->disposition != UMI_INTEGRATION_LAUNCH_OBSERVED_STARTING)
        return UMI_STATUS_INVALID_STATE;
    item->disposition = UMI_INTEGRATION_LAUNCH_OBSERVED_STOPPED;
    RecomputeEvidence(runtime);
    return UMI_STATUS_OK;
}

/* Exit codes are provided by the REAL owning supervisor. The supervisor
 * must reject stale process tokens BEFORE invoking this translator. A clean
 * exit is not a failure, but cannot leave a suite spuriously RUNNING. */
UmiStatus umi_integration_suite_runtime_mark_exit(
    UmiIntegrationSuiteRuntime *runtime,
    const char *application_id,
    int exit_code)
{
    if (exit_code == 0)
        return umi_integration_suite_runtime_mark_stopped(runtime, application_id);
    return umi_integration_suite_runtime_mark_failed(runtime, application_id);
}

/*
 * Provide the integration suite runtime is usable operation used by this module and its
 * client applications.
 */
bool umi_integration_suite_runtime_is_usable(
    const UmiIntegrationSuiteRuntime *runtime)
{
    size_t index;
    size_t required_total = 0U;
    if (runtime == NULL || runtime->plan.count > UMI_INTEGRATION_MAX_MEMBERS ||
        runtime->plan.missing_required != 0U || runtime->failed_required != 0U ||
        (runtime->state != UMI_INTEGRATION_SUITE_RUNNING &&
         runtime->state != UMI_INTEGRATION_SUITE_DEGRADED))
        return false;
    for (index = 0U; index < runtime->plan.count; ++index)
        if (runtime->plan.items[index].kind == UMI_INTEGRATION_DEPENDENCY_REQUIRED)
            ++required_total;
    return runtime->running_required == required_total &&
        runtime->running_required + runtime->running_optional > 0U;
}
