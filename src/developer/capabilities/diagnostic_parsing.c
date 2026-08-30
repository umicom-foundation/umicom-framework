/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/diagnostic_parsing.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Diagnostic Parsing developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/diagnostic_parsing.h"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_diagnostic_parsing(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_DIAGNOSTIC_PARSING,
        .title = "Diagnostic Parsing",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_BUILD,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Convert compiler and linker output into portable problems with source locations.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
