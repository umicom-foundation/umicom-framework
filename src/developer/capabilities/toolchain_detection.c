/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/toolchain_detection.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Toolchain Detection developer capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/toolchain_detection.h"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_toolchain_detection(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_TOOLCHAIN_DETECTION,
        .title = "Toolchain Detection",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_CONFIGURATION,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Resolve available C, C++ and assembly toolchains through portable Framework contracts.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
