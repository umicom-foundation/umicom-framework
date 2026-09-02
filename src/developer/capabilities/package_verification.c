/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/package_verification.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Package Verification developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/package_verification.h"

/*
 * Provide the developer capability package verification operation used by this module and
 * its client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_package_verification(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_PACKAGE_VERIFICATION,
        .title = "Package Verification",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_DELIVERY,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Verify integrity, compatibility and policy before promotion.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
