/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/policy.c
 * PURPOSE: Implement safe, deterministic build execution policy defaults.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/build/policy.h"

#include <stdio.h>
#include <string.h>

void umi_build_execution_policy_init(UmiBuildExecutionPolicy *policy)
{
    if (policy == NULL) return;
    memset(policy, 0, sizeof(*policy));
    policy->structure_size = (uint32_t)sizeof(*policy);
    policy->api_version = UMI_BUILD_POLICY_API_VERSION;
    policy->default_timeout_ms = 0U;
    policy->maximum_attempts = 2U;
    policy->maximum_parallel_jobs = UMI_BUILD_SAFE_PARALLEL_JOB_LIMIT;
    policy->incremental_configure = 1;
    policy->incremental_build = 1;
}

UmiStatus umi_build_execution_policy_validate(
    const UmiBuildExecutionPolicy *policy,
    char *out_message,
    size_t message_capacity)
{
    UmiStatus status = UMI_STATUS_OK;
    const char *message = "Build execution policy is valid";
    if (policy == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (policy->structure_size < sizeof(*policy) ||
        policy->api_version != UMI_BUILD_POLICY_API_VERSION) {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Build execution policy version is incompatible";
    } else if (policy->maximum_attempts == 0U) {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Build execution attempts must be greater than zero";
    } else if (policy->maximum_parallel_jobs == 0U ||
               policy->maximum_parallel_jobs >
                   UMI_BUILD_SAFE_PARALLEL_JOB_LIMIT) {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Build parallelism must be between 1 and 32 jobs";
    }
    if (out_message != NULL && message_capacity > 0U)
        (void)snprintf(out_message, message_capacity, "%s", message);
    return status;
}

unsigned umi_build_policy_safe_parallel_jobs(unsigned requested_jobs,
                                             unsigned available_processors,
                                             unsigned policy_limit)
{
    unsigned result = requested_jobs;
    unsigned limit = policy_limit;
    if (limit == 0U || limit > UMI_BUILD_SAFE_PARALLEL_JOB_LIMIT)
        limit = UMI_BUILD_SAFE_PARALLEL_JOB_LIMIT;
    if (result == 0U) result = available_processors;
    if (result == 0U) result = 1U;
    if (available_processors > 0U && result > available_processors)
        result = available_processors;
    if (result > limit) result = limit;
    return result;
}
