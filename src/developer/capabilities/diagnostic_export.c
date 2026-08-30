/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/diagnostic_export.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Diagnostic Export developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/diagnostic_export.h"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_diagnostic_export(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_DIAGNOSTIC_EXPORT,
        .title = "Diagnostic Export",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_DIAGNOSTICS,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Export selected evidence with privacy filtering and deterministic naming.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
