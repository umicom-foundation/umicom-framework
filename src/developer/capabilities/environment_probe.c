/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/environment_probe.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Environment Probe developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/environment_probe.h"

/*
 * Provide the developer capability environment probe operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_environment_probe(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_ENVIRONMENT_PROBE,
        .title = "Environment Probe",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_CONFIGURATION,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Capture compiler, generator, SDK and environment evidence before an operation begins.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
