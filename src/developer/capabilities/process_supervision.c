/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/process_supervision.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Process Supervision developer capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/process_supervision.h"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_process_supervision(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_PROCESS_SUPERVISION,
        .title = "Process Supervision",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_EXECUTION,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Track child lifetime, exit state, cancellation and restart policy.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
