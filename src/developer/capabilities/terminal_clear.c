/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/terminal_clear.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Terminal Clear developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/terminal_clear.h"

/*
 * Release or reset state held by developer capability terminal so the same storage can be
 * reused safely.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_terminal_clear(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_TERMINAL_CLEAR,
        .title = "Terminal Clear",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_TERMINAL,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Clear a transcript without destroying the owning session or environment.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
