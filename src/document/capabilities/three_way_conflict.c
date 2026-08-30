/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/three_way_conflict.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Three Way Conflict document capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/three_way_conflict.h"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_three_way_conflict(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_THREE_WAY_CONFLICT,
        .title = "Three Way Conflict",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_CONFLICT,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_PLANNED,
        .summary = "Defines external-change detection and explicit conflict resolution without silent data loss.",
        .provider_role = "framework-extension",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI,
        .priority = 510
    };
    return &descriptor;
}
