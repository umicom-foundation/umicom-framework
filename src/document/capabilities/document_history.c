/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/document_history.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Document History document capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/document_history.h"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_document_history(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_DOCUMENT_HISTORY,
        .title = "Document History",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_NAVIGATION,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_PLANNED,
        .summary = "Defines reusable document search and navigation behaviour independent from a native editor widget.",
        .provider_role = "framework-extension",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI,
        .priority = 720
    };
    return &descriptor;
}
