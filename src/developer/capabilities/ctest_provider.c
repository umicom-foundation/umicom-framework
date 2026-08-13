/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/ctest_provider.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the CTest Provider developer capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/ctest_provider.h"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_ctest_provider(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_CTEST_PROVIDER,
        .title = "CTest Provider",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_INTEGRATION,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Discover and execute CTest tests with normalised results.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
