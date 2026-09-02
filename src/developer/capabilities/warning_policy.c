/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/warning_policy.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Warning Policy developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/warning_policy.h"

/*
 * Provide the developer capability warning policy operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_warning_policy(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_WARNING_POLICY,
        .title = "Warning Policy",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_BUILD,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Define strict warning classes and escalation rules for owned source targets.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
