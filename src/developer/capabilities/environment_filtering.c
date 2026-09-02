/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/environment_filtering.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Environment Filtering developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/environment_filtering.h"

/*
 * Provide the developer capability environment filtering operation used by this module and
 * its client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_environment_filtering(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_ENVIRONMENT_FILTERING,
        .title = "Environment Filtering",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_SECURITY,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Prevent sensitive inherited variables from leaking to child processes.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
