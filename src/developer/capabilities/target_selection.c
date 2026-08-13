/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/target_selection.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Target Selection developer capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/target_selection.h"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_target_selection(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_TARGET_SELECTION,
        .title = "Target Selection",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_BUILD,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Select one or more named targets without rebuilding unrelated application products.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
