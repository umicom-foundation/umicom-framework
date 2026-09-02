/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/health_snapshot.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Health Snapshot developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/health_snapshot.h"

/*
 * Provide the developer capability health snapshot operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_health_snapshot(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_HEALTH_SNAPSHOT,
        .title = "Health Snapshot",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_DIAGNOSTICS,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Produce a stable machine-readable health view for support and automation.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
