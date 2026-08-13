/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/readiness_reporting.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Readiness Reporting developer capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/readiness_reporting.h"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_readiness_reporting(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_READINESS_REPORTING,
        .title = "Readiness Reporting",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_DIAGNOSTICS,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Summarise whether dependencies and services are ready for an operation.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
