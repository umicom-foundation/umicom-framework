/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/environment_overlay.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Environment Overlay developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/environment_overlay.h"

/*
 * Provide the developer capability environment overlay operation used by this module and
 * its client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_environment_overlay(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_ENVIRONMENT_OVERLAY,
        .title = "Environment Overlay",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_EXECUTION,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Merge inherited, profile and launch-specific environment variables safely.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
