/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/crash_reporting.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Crash Reporting developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/crash_reporting.h"

/*
 * Provide the developer capability crash reporting operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_crash_reporting(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_CRASH_REPORTING,
        .title = "Crash Reporting",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_DIAGNOSTICS,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Capture bounded failure context without exposing secrets by default.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
