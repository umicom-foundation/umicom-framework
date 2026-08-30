/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/windows_ucrt.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Windows UCRT developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/windows_ucrt.h"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_windows_ucrt(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_WINDOWS_UCRT,
        .title = "Windows UCRT",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_PORTABILITY,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Support GNU UCRT64 builds and Windows process, path and plug-in ABI behaviour.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
