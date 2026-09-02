/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/container_runner.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Container Runner developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/container_runner.h"

/*
 * Provide the developer capability container runner operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_container_runner(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_CONTAINER_RUNNER,
        .title = "Container Runner",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_INTEGRATION,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Define an isolated process provider for reproducible build and test workloads.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
