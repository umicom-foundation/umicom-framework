/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/reopen_document.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Reopen Document document capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/reopen_document.h"

/*
 * Provide the document capability reopen document operation used by this module and its
 * client applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_reopen_document(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_REOPEN_DOCUMENT,
        .title = "Reopen Document",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_LIFECYCLE,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_PLANNED,
        .summary = "Defines a reusable document lifecycle contract with deterministic ownership and state transitions.",
        .provider_role = "framework-extension",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI,
        .priority = 40
    };
    return &descriptor;
}
