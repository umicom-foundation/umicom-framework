/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/distributed_tracing.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Distributed Tracing developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/distributed_tracing.h"

/*
 * Provide the developer capability distributed tracing operation used by this module and
 * its client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_distributed_tracing(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_DISTRIBUTED_TRACING,
        .title = "Distributed Tracing",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_DIAGNOSTICS,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Correlate nested operations across providers and application boundaries.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
