/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/reload_resolution.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Reload Resolution document capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/reload_resolution.h"

/*
 * Provide the document capability reload resolution operation used by this module and its
 * client applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_reload_resolution(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_RELOAD_RESOLUTION,
        .title = "Reload Resolution",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_CONFLICT,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_STABLE,
        .summary = "Defines external-change detection and explicit conflict resolution without silent data loss.",
        .provider_role = "framework-core",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI | UMI_DOCUMENT_CAPABILITY_FLAG_IMPLEMENTED,
        .priority = 530
    };
    return &descriptor;
}
