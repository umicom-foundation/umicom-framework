/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/memory_budget.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Memory Budget developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/memory_budget.h"

/*
 * Provide the developer capability memory budget operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_memory_budget(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_MEMORY_BUDGET,
        .title = "Memory Budget",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_PERFORMANCE,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Expose bounded allocations for indexes, histories, transcripts and diagnostics.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
