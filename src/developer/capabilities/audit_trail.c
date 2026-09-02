/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/audit_trail.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Audit Trail developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/audit_trail.h"

/*
 * Provide the developer capability audit trail operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_audit_trail(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_AUDIT_TRAIL,
        .title = "Audit Trail",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_SECURITY,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Record security-sensitive developer actions with actor and correlation context.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
