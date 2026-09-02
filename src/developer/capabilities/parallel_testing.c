/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/parallel_testing.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Parallel Testing developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/parallel_testing.h"

/*
 * Provide the developer capability parallel testing operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_parallel_testing(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_PARALLEL_TESTING,
        .title = "Parallel Testing",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_TESTING,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Execute independent tests concurrently while respecting fixtures and resource locks.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
