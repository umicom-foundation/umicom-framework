/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/install_prefix.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Install Prefix developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/install_prefix.h"

/*
 * Provide the developer capability install prefix operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_install_prefix(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_INSTALL_PREFIX,
        .title = "Install Prefix",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_DELIVERY,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Select a local or staged installation destination through a validated profile.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
