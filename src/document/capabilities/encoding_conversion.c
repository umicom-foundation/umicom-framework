/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/encoding_conversion.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Encoding Conversion document capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/encoding_conversion.h"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_encoding_conversion(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_ENCODING_CONVERSION,
        .title = "Encoding Conversion",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_ENCODING,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_STABLE,
        .summary = "Defines explicit text encoding detection, validation and conversion semantics for loss-aware editing.",
        .provider_role = "framework-core",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI | UMI_DOCUMENT_CAPABILITY_FLAG_IMPLEMENTED,
        .priority = 310
    };
    return &descriptor;
}
