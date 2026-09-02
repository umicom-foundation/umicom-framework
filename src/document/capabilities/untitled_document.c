/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/untitled_document.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Untitled Document document capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/untitled_document.h"

/*
 * Provide the document capability untitled document operation used by this module and its
 * client applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_untitled_document(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_UNTITLED_DOCUMENT,
        .title = "Untitled Document",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_LIFECYCLE,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_STABLE,
        .summary = "Defines a reusable document lifecycle contract with deterministic ownership and state transitions.",
        .provider_role = "framework-core",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI | UMI_DOCUMENT_CAPABILITY_FLAG_IMPLEMENTED,
        .priority = 10
    };
    return &descriptor;
}
