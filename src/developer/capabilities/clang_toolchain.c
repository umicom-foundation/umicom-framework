/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/clang_toolchain.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Clang Toolchain developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/clang_toolchain.h"

/*
 * Provide the developer capability clang toolchain operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_clang_toolchain(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_CLANG_TOOLCHAIN,
        .title = "Clang Toolchain",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_INTEGRATION,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Describe Clang compiler, linker, diagnostics and tool discovery.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
