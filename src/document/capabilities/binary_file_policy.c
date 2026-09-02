/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/binary_file_policy.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Binary File Policy document capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/binary_file_policy.h"

/*
 * Provide the document capability binary file policy operation used by this module and its
 * client applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_binary_file_policy(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_BINARY_FILE_POLICY,
        .title = "Binary File Policy",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_SECURITY,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_STABLE,
        .summary = "Defines document I/O security policy for trust, paths, permissions, limits, auditing and redaction.",
        .provider_role = "framework-core",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI | UMI_DOCUMENT_CAPABILITY_FLAG_SECURITY_SENSITIVE | UMI_DOCUMENT_CAPABILITY_FLAG_IMPLEMENTED,
        .priority = 770
    };
    return &descriptor;
}
