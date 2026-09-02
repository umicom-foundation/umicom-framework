/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/macos.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the macOS developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/macos.h"

/*
 * Provide the developer capability macos operation used by this module and its client
 * applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_macos(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_MACOS,
        .title = "macOS",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_PORTABILITY,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Define the Darwin process, bundle, signing and filesystem portability boundary.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
