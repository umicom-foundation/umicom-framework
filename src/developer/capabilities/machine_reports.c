/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/machine_reports.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Machine Reports developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/machine_reports.h"

/*
 * Provide the developer capability machine reports operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_machine_reports(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_MACHINE_REPORTS,
        .title = "Machine Reports",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_TESTING,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Export stable JUnit, JSON and Framework-native result summaries.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
