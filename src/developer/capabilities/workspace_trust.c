/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/workspace_trust.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Workspace Trust developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/workspace_trust.h"

/*
 * Provide the developer capability workspace trust operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_workspace_trust(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_WORKSPACE_TRUST,
        .title = "Workspace Trust",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_SECURITY,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Gate executable workspace actions on an explicit trust decision.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
