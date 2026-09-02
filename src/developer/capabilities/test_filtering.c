/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/test_filtering.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Test Filtering developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/test_filtering.h"

/*
 * Provide the developer capability test filtering operation used by this module and its
 * client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_test_filtering(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_TEST_FILTERING,
        .title = "Test Filtering",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_TESTING,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Select tests by name, label, path, owner or latest result.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
