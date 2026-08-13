/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/deployment_centre.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Deployment Centre developer capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/deployment_centre.h"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_deployment_centre(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_DEPLOYMENT_CENTRE,
        .title = "Deployment Centre",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_EXPERIENCE,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Inspect installation plans, evidence, health and rollback candidates.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
