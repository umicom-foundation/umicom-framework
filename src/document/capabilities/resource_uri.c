/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/resource_uri.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Resource Uri document capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/resource_uri.h"

/*
 * Provide the document capability resource uri operation used by this module and its
 * client applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_resource_uri(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_RESOURCE_URI,
        .title = "Resource Uri",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_IDENTITY,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_STABLE,
        .summary = "Defines stable resource identity and metadata required for duplicate detection and safe persistence.",
        .provider_role = "framework-core",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI | UMI_DOCUMENT_CAPABILITY_FLAG_PERSISTED | UMI_DOCUMENT_CAPABILITY_FLAG_IMPLEMENTED,
        .priority = 90
    };
    return &descriptor;
}
