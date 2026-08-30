/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/read_only_document.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Read Only Document document capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/read_only_document.h"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_read_only_document(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_READ_ONLY_DOCUMENT,
        .title = "Read Only Document",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_LIFECYCLE,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_PLANNED,
        .summary = "Defines a reusable document lifecycle contract with deterministic ownership and state transitions.",
        .provider_role = "framework-extension",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI,
        .priority = 60
    };
    return &descriptor;
}
