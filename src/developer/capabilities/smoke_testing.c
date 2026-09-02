/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/smoke_testing.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Smoke Testing developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/smoke_testing.h"

/*
 * Provide the developer capability smoke testing operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_smoke_testing(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_SMOKE_TESTING,
        .title = "Smoke Testing",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_DELIVERY,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Run post-install readiness checks and retain their output as release evidence.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
