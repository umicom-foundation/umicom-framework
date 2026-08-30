/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/working_directory_sync.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Working Directory Sync developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/working_directory_sync.h"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_working_directory_sync(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_WORKING_DIRECTORY_SYNC,
        .title = "Working Directory Sync",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_TERMINAL,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Synchronise a session directory with an active workspace under policy.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
