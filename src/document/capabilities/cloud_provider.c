/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/cloud_provider.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Cloud Provider document capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/cloud_provider.h"

/*
 * Provide the document capability cloud provider operation used by this module and its
 * client applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_cloud_provider(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_CLOUD_PROVIDER,
        .title = "Cloud Provider",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_PROVIDER,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_PLANNED,
        .summary = "Defines a provider-neutral storage boundary so applications never embed backend-specific file logic.",
        .provider_role = "framework-extension",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI,
        .priority = 230
    };
    return &descriptor;
}
