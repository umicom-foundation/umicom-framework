/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/compare_resolution.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Compare Resolution document capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/compare_resolution.h"

/*
 * Provide the document capability compare resolution operation used by this module and its
 * client applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_compare_resolution(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_COMPARE_RESOLUTION,
        .title = "Compare Resolution",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_CONFLICT,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_STABLE,
        .summary = "Defines external-change detection and explicit conflict resolution without silent data loss.",
        .provider_role = "framework-core",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI | UMI_DOCUMENT_CAPABILITY_FLAG_IMPLEMENTED,
        .priority = 540
    };
    return &descriptor;
}
