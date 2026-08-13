/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/utf16be_encoding.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Utf16be Encoding document capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/utf16be_encoding.h"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_utf16be_encoding(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_UTF16BE_ENCODING,
        .title = "Utf16be Encoding",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_ENCODING,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_STABLE,
        .summary = "Defines explicit text encoding detection, validation and conversion semantics for loss-aware editing.",
        .provider_role = "framework-core",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI | UMI_DOCUMENT_CAPABILITY_FLAG_IMPLEMENTED,
        .priority = 280
    };
    return &descriptor;
}
