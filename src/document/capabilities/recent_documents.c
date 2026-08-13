/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/recent_documents.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Recent Documents document capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/recent_documents.h"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_recent_documents(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_RECENT_DOCUMENTS,
        .title = "Recent Documents",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_NAVIGATION,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_STABLE,
        .summary = "Defines reusable document search and navigation behaviour independent from a native editor widget.",
        .provider_role = "framework-core",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI | UMI_DOCUMENT_CAPABILITY_FLAG_IMPLEMENTED,
        .priority = 710
    };
    return &descriptor;
}
