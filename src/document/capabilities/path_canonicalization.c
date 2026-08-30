/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/path_canonicalization.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Path Canonicalization document capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/path_canonicalization.h"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_path_canonicalization(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_PATH_CANONICALIZATION,
        .title = "Path Canonicalization",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_IDENTITY,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_STABLE,
        .summary = "Defines stable resource identity and metadata required for duplicate detection and safe persistence.",
        .provider_role = "framework-core",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI | UMI_DOCUMENT_CAPABILITY_FLAG_PERSISTED | UMI_DOCUMENT_CAPABILITY_FLAG_IMPLEMENTED,
        .priority = 110
    };
    return &descriptor;
}
