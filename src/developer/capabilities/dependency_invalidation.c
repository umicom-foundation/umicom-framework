/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/dependency_invalidation.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Dependency Invalidation developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/dependency_invalidation.h"

/*
 * Provide the developer capability dependency invalidation operation used by this module
 * and its client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_dependency_invalidation(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_DEPENDENCY_INVALIDATION,
        .title = "Dependency Invalidation",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_PERFORMANCE,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Invalidate only nodes affected by changed inputs or settings.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
