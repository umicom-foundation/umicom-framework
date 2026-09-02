/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/dirty_conflict.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Dirty Conflict document capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/dirty_conflict.h"

/*
 * Provide the document capability dirty conflict operation used by this module and its
 * client applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_dirty_conflict(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_DIRTY_CONFLICT,
        .title = "Dirty Conflict",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_CONFLICT,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_STABLE,
        .summary = "Defines external-change detection and explicit conflict resolution without silent data loss.",
        .provider_role = "framework-core",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI | UMI_DOCUMENT_CAPABILITY_FLAG_IMPLEMENTED,
        .priority = 500
    };
    return &descriptor;
}
