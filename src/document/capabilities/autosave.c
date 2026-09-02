/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/autosave.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Autosave document capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/autosave.h"

/*
 * Provide the document capability autosave operation used by this module and its client
 * applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_autosave(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_AUTOSAVE,
        .title = "Autosave",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_RECOVERY,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_PLANNED,
        .summary = "Defines recoverable working-copy state and restart behaviour after normal or abnormal termination.",
        .provider_role = "framework-extension",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI | UMI_DOCUMENT_CAPABILITY_FLAG_SECURITY_SENSITIVE | UMI_DOCUMENT_CAPABILITY_FLAG_PERSISTED,
        .priority = 410
    };
    return &descriptor;
}
