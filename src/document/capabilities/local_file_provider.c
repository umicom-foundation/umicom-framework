/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/local_file_provider.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Local File Provider document capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/local_file_provider.h"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_local_file_provider(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_LOCAL_FILE_PROVIDER,
        .title = "Local File Provider",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_PROVIDER,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_STABLE,
        .summary = "Defines a provider-neutral storage boundary so applications never embed backend-specific file logic.",
        .provider_role = "framework-core",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI | UMI_DOCUMENT_CAPABILITY_FLAG_IMPLEMENTED,
        .priority = 170
    };
    return &descriptor;
}
