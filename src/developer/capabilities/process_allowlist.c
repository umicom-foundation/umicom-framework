/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/process_allowlist.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Process Allowlist developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/process_allowlist.h"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_process_allowlist(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_PROCESS_ALLOWLIST,
        .title = "Process Allowlist",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_SECURITY,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Restrict executable providers when a deployment policy requires it.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
