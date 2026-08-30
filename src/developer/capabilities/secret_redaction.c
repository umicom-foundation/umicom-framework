/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/secret_redaction.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Secret Redaction developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/secret_redaction.h"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_secret_redaction(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_SECRET_REDACTION,
        .title = "Secret Redaction",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_SECURITY,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Remove configured credentials and sensitive values from captured output.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
