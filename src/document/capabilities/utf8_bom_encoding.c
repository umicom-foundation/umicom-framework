/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/utf8_bom_encoding.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Utf8 Bom Encoding document capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/utf8_bom_encoding.h"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_utf8_bom_encoding(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_UTF8_BOM_ENCODING,
        .title = "Utf8 Bom Encoding",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_ENCODING,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_STABLE,
        .summary = "Defines explicit text encoding detection, validation and conversion semantics for loss-aware editing.",
        .provider_role = "framework-core",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI | UMI_DOCUMENT_CAPABILITY_FLAG_IMPLEMENTED,
        .priority = 260
    };
    return &descriptor;
}
