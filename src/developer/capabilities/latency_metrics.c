/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/latency_metrics.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Latency Metrics developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/latency_metrics.h"

/*
 * Provide the developer capability latency metrics operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_latency_metrics(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_LATENCY_METRICS,
        .title = "Latency Metrics",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_PERFORMANCE,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Measure operation queue, startup and completion latency consistently.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
