/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/launch_configuration.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Launch Configuration developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/launch_configuration.h"

/*
 * Provide the developer capability launch configuration operation used by this module and
 * its client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_launch_configuration(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_LAUNCH_CONFIGURATION,
        .title = "Launch Configuration",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_EXECUTION,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Describe executable, arguments, environment and working directory as reusable data.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
