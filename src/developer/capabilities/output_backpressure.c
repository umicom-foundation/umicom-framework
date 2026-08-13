/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/output_backpressure.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Output Backpressure developer capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/output_backpressure.h"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_output_backpressure(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_OUTPUT_BACKPRESSURE,
        .title = "Output Backpressure",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_PERFORMANCE,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Bound producer output and protect UI consumers from unbounded queues.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
