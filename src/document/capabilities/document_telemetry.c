/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/document_telemetry.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Document Telemetry document capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/document_telemetry.h"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_document_telemetry(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_DOCUMENT_TELEMETRY,
        .title = "Document Telemetry",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_INTEGRATION,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_STABLE,
        .summary = "Defines an adapter boundary connecting the document core to UI, language, VCS, tests and telemetry.",
        .provider_role = "framework-core",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI | UMI_DOCUMENT_CAPABILITY_FLAG_IMPLEMENTED,
        .priority = 960
    };
    return &descriptor;
}
