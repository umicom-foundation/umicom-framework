/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/display_name.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Display Name document capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/display_name.h"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_display_name(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_DISPLAY_NAME,
        .title = "Display Name",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_IDENTITY,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_STABLE,
        .summary = "Defines stable resource identity and metadata required for duplicate detection and safe persistence.",
        .provider_role = "framework-core",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI | UMI_DOCUMENT_CAPABILITY_FLAG_PERSISTED | UMI_DOCUMENT_CAPABILITY_FLAG_IMPLEMENTED,
        .priority = 140
    };
    return &descriptor;
}
