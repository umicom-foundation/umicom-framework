/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/terminal_split.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Terminal Split developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/terminal_split.h"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_terminal_split(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_TERMINAL_SPLIT,
        .title = "Terminal Split",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_TERMINAL,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Describe reusable horizontal and vertical terminal arrangements.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
