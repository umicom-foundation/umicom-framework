/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/coverage_capture.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Coverage Capture developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/coverage_capture.h"

/*
 * Provide the developer capability coverage capture operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_coverage_capture(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_COVERAGE_CAPTURE,
        .title = "Coverage Capture",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_TESTING,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Collect source coverage artifacts and associate them with a tested revision.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
