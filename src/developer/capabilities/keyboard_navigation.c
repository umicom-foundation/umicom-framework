/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/keyboard_navigation.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Keyboard Navigation developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/keyboard_navigation.h"

/*
 * Provide the developer capability keyboard navigation operation used by this module and
 * its client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_keyboard_navigation(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_KEYBOARD_NAVIGATION,
        .title = "Keyboard Navigation",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_EXPERIENCE,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Provide complete action access through stable shortcuts and focus order.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
