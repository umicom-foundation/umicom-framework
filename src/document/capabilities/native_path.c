/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/native_path.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Native Path document capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/native_path.h"

/*
 * Provide the document capability native path operation used by this module and its client
 * applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_native_path(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_NATIVE_PATH,
        .title = "Native Path",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_IDENTITY,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_STABLE,
        .summary = "Defines stable resource identity and metadata required for duplicate detection and safe persistence.",
        .provider_role = "framework-core",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI | UMI_DOCUMENT_CAPABILITY_FLAG_PERSISTED | UMI_DOCUMENT_CAPABILITY_FLAG_IMPLEMENTED,
        .priority = 100
    };
    return &descriptor;
}
