/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/linux_glibc.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Linux GLIBC developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/linux_glibc.h"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_linux_glibc(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_LINUX_GLIBC,
        .title = "Linux GLIBC",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_PORTABILITY,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Support GNU and Clang builds on mainstream GLIBC Linux systems.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
