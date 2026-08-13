/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/sanitizer_profile.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Sanitizer Profile developer capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/sanitizer_profile.h"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_sanitizer_profile(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_SANITIZER_PROFILE,
        .title = "Sanitizer Profile",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_BUILD,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Select supported runtime instrumentation without hard-coding compiler arguments in products.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
