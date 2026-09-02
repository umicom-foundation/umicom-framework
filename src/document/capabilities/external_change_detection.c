/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/external_change_detection.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the External Change Detection document capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/external_change_detection.h"

/*
 * Provide the document capability external change detection operation used by this module
 * and its client applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_external_change_detection(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_EXTERNAL_CHANGE_DETECTION,
        .title = "External Change Detection",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_CONFLICT,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_STABLE,
        .summary = "Defines external-change detection and explicit conflict resolution without silent data loss.",
        .provider_role = "framework-core",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI | UMI_DOCUMENT_CAPABILITY_FLAG_IMPLEMENTED,
        .priority = 490
    };
    return &descriptor;
}
