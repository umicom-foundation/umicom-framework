/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/notifications.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Notifications developer capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/notifications.h"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_notifications(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_NOTIFICATIONS,
        .title = "Notifications",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_EXPERIENCE,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Publish actionable completion and failure information without blocking work.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
