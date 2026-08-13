/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/argument_quoting.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Argument Quoting developer capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/argument_quoting.h"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_argument_quoting(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_ARGUMENT_QUOTING,
        .title = "Argument Quoting",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_EXECUTION,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Preserve argument boundaries across Windows and POSIX process APIs.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
