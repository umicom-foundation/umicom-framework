/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/workspace_discovery.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Workspace Discovery developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/workspace_discovery.h"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_workspace_discovery(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_WORKSPACE_DISCOVERY,
        .title = "Workspace Discovery",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_CONFIGURATION,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Discover and validate the workspace roots consumed by configure, build, test and run operations.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
