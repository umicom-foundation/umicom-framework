/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/encoding_detection.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Encoding Detection document capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/encoding_detection.h"

/*
 * Provide the document capability encoding detection operation used by this module and its
 * client applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_encoding_detection(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_ENCODING_DETECTION,
        .title = "Encoding Detection",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_ENCODING,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_STABLE,
        .summary = "Defines explicit text encoding detection, validation and conversion semantics for loss-aware editing.",
        .provider_role = "framework-core",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI | UMI_DOCUMENT_CAPABILITY_FLAG_IMPLEMENTED,
        .priority = 300
    };
    return &descriptor;
}
