/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/runtime_metrics.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Runtime Metrics developer capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/runtime_metrics.h"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_runtime_metrics(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_RUNTIME_METRICS,
        .title = "Runtime Metrics",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_DIAGNOSTICS,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Collect counters, gauges and timings from developer operations.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
