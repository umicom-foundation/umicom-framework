/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/file_lock.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the File Lock document capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/file_lock.h"

/*
 * Provide the document capability file lock operation used by this module and its client
 * applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_file_lock(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_FILE_LOCK,
        .title = "File Lock",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_CONFLICT,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_PLANNED,
        .summary = "Defines external-change detection and explicit conflict resolution without silent data loss.",
        .provider_role = "framework-extension",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI,
        .priority = 550
    };
    return &descriptor;
}
