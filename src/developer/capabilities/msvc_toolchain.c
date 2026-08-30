/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/msvc_toolchain.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the MSVC Toolchain developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/msvc_toolchain.h"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_msvc_toolchain(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_MSVC_TOOLCHAIN,
        .title = "MSVC Toolchain",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_INTEGRATION,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Describe MSVC compiler, linker, environment and diagnostic discovery.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
