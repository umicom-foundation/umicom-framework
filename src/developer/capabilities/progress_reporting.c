/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/progress_reporting.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Progress Reporting developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/progress_reporting.h"

/*
 * Provide the developer capability progress reporting operation used by this module and
 * its client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_progress_reporting(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_PROGRESS_REPORTING,
        .title = "Progress Reporting",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_DIAGNOSTICS,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Report determinate and indeterminate operation progress through shared services.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
