/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/vcs_diff_bridge.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Vcs Diff Bridge document capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/vcs_diff_bridge.h"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_vcs_diff_bridge(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_VCS_DIFF_BRIDGE,
        .title = "Vcs Diff Bridge",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_INTEGRATION,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_PLANNED,
        .summary = "Defines an adapter boundary connecting the document core to UI, language, VCS, tests and telemetry.",
        .provider_role = "framework-extension",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI,
        .priority = 930
    };
    return &descriptor;
}
