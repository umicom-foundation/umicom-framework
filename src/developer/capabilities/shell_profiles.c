/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/shell_profiles.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Shell Profiles developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/shell_profiles.h"

/*
 * Provide the developer capability shell profiles operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_shell_profiles(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_SHELL_PROFILES,
        .title = "Shell Profiles",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_TERMINAL,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Describe PowerShell, Bash and other shells through explicit executable profiles.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
