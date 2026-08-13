/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/build_cancellation.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Build Cancellation developer capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/build_cancellation.h"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_build_cancellation(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_BUILD_CANCELLATION,
        .title = "Build Cancellation",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_BUILD,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Request cooperative cancellation and retain a truthful terminal operation state.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
