/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/build_cache.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Build Cache developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/build_cache.h"

/*
 * Provide the developer capability build cache operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_build_cache(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_BUILD_CACHE,
        .title = "Build Cache",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_PERFORMANCE,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Reuse valid build outputs with explicit key and provenance contracts.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
