/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/compile_commands.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Compile Commands developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/compile_commands.h"

/*
 * Provide the developer capability compile commands operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_compile_commands(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_COMPILE_COMMANDS,
        .title = "Compile Commands",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_BUILD,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Generate and index compiler invocations for language intelligence and diagnostics.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
