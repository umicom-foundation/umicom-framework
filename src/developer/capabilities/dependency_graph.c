/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/capabilities/dependency_graph.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Dependency Graph developer capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/capabilities/dependency_graph.h"

const UmiDeveloperCapabilityDescriptor *
umi_developer_capability_dependency_graph(void)
{
    static const UmiDeveloperCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDeveloperCapabilityDescriptor),
        .api_version = UMI_DEVELOPER_CAPABILITY_API_VERSION,
        .capability_id = UMI_DEVELOPER_CAPABILITY_DEPENDENCY_GRAPH,
        .title = "Dependency Graph",
        .category = UMI_DEVELOPER_CAPABILITY_CATEGORY_CONFIGURATION,
        .maturity = UMI_DEVELOPER_CAPABILITY_MATURITY_PLANNED,
        .summary = "Model configuration dependencies and explain why one setting enables or blocks another.",
        .provider_role = "framework-developer-platform",
        .flags = UMI_DEVELOPER_CAPABILITY_FLAG_HEADLESS |
                 UMI_DEVELOPER_CAPABILITY_FLAG_GUI,
        .priority = 500
    };
    return &descriptor;
}
