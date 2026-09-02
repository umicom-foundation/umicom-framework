/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/feature_option_matrix.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Feature Option Matrix developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/feature_option_matrix.h"

/*
 * Provide the developer capability feature option matrix operation used by this module and
 * its client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_feature_option_matrix(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_FEATURE_OPTION_MATRIX,
        .title = "Feature Option Matrix",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_CONFIGURATION,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Expose supported optional capabilities and their resolved on, off or unavailable state.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
