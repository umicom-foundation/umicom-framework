/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/policy.h
 *
 * PURPOSE:
 *   Define bounded execution defaults shared by the build graph, providers,
 *   Studio and CI.  The policy keeps retries and Ninja parallelism explicit.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BUILD_POLICY_H
#define UMICOM_BUILD_POLICY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_BUILD_POLICY_API_VERSION 1U
#define UMI_BUILD_SAFE_PARALLEL_JOB_LIMIT 32U

typedef struct UmiBuildExecutionPolicy {
    uint32_t structure_size;
    uint32_t api_version;
    uint32_t default_timeout_ms;
    uint32_t maximum_attempts;
    unsigned maximum_parallel_jobs;
    int incremental_configure;
    int incremental_build;
} UmiBuildExecutionPolicy;

void umi_build_execution_policy_init(UmiBuildExecutionPolicy *policy);
UmiStatus umi_build_execution_policy_validate(
    const UmiBuildExecutionPolicy *policy,
    char *out_message,
    size_t message_capacity);
unsigned umi_build_policy_safe_parallel_jobs(unsigned requested_jobs,
                                             unsigned available_processors,
                                             unsigned policy_limit);

#ifdef __cplusplus
}
#endif
#endif
