/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/gcc_toolchain.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the GCC Toolchain developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/gcc_toolchain.h"

/*
 * Provide the developer capability gcc toolchain operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_gcc_toolchain(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_GCC_TOOLCHAIN,
        .title = "GCC Toolchain",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_INTEGRATION,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Describe GCC compiler, linker, diagnostics and tool discovery.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
