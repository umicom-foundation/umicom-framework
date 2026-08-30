/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/parallel_job_budget.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Parallel Job Budget developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/parallel_job_budget.h"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_parallel_job_budget(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_PARALLEL_JOB_BUDGET,
        .title = "Parallel Job Budget",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_PERFORMANCE,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Resolve safe concurrency from profile, host capacity and policy.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
