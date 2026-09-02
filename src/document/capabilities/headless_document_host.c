/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/headless_document_host.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Headless Document Host document capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/headless_document_host.h"

/*
 * Provide the document capability headless document host operation used by this module and
 * its client applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_headless_document_host(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_HEADLESS_DOCUMENT_HOST,
        .title = "Headless Document Host",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_INTEGRATION,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_STABLE,
        .summary = "Defines an adapter boundary connecting the document core to UI, language, VCS, tests and telemetry.",
        .provider_role = "framework-core",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI | UMI_DOCUMENT_CAPABILITY_FLAG_IMPLEMENTED,
        .priority = 900
    };
    return &descriptor;
}
