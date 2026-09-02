/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/path_normalisation.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Path Normalisation developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/path_normalisation.h"

/*
 * Provide the developer capability path normalisation operation used by this module and
 * its client applications.
 */
const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_path_normalisation(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_PATH_NORMALISATION,
        .title = "Path Normalisation",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_PORTABILITY,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Normalise separators, roots and case policy without losing native identity.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
