/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/capability_negotiation.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Capability Negotiation developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/capability_negotiation.h"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_capability_negotiation(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_CAPABILITY_NEGOTIATION,
        .title = "Capability Negotiation",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_PORTABILITY,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Report available host features before enabling provider-specific actions.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
