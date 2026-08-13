/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/continuous_testing.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Continuous Testing developer capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/continuous_testing.h"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_continuous_testing(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_CONTINUOUS_TESTING,
        .title = "Continuous Testing",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_TESTING,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Schedule affected tests after trusted workspace changes with debouncing.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
