/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/memory_mapping.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Memory Mapping document capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/memory_mapping.h"

/*
 * Provide the document capability memory mapping operation used by this module and its
 * client applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_memory_mapping(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_MEMORY_MAPPING,
        .title = "Memory Mapping",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_PERFORMANCE,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_PLANNED,
        .summary = "Defines scalable and cancellable document I/O behaviour for large or remote resources.",
        .provider_role = "framework-extension",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI,
        .priority = 810
    };
    return &descriptor;
}
