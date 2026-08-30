/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/artifact_signing.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Artifact Signing developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/artifact_signing.h"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_artifact_signing(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_ARTIFACT_SIGNING,
        .title = "Artifact Signing",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_DELIVERY,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Attach provider signatures and signing evidence to release artifacts.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
