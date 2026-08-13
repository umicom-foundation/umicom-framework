/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/watch_debounce.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Watch Debounce developer capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/watch_debounce.h"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_watch_debounce(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_WATCH_DEBOUNCE,
        .title = "Watch Debounce",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_PERFORMANCE,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Coalesce noisy filesystem changes before scheduling developer work.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
