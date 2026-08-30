/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/chunked_transport.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Chunked Transport document capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/chunked_transport.h"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_chunked_transport(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_CHUNKED_TRANSPORT,
        .title = "Chunked Transport",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_PERFORMANCE,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_PLANNED,
        .summary = "Defines scalable and cancellable document I/O behaviour for large or remote resources.",
        .provider_role = "framework-extension",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI,
        .priority = 870
    };
    return &descriptor;
}
