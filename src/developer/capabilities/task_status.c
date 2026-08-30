/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/task_status.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Task Status developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/task_status.h"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_task_status(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_TASK_STATUS,
        .title = "Task Status",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_EXPERIENCE,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Present queued, running, completed and failed operation state consistently.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
