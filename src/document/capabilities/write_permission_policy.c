/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/write_permission_policy.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Write Permission Policy document capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/write_permission_policy.h"

/*
 * Provide the document capability write permission policy operation used by this module
 * and its client applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_write_permission_policy(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_WRITE_PERMISSION_POLICY,
        .title = "Write Permission Policy",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_SECURITY,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_STABLE,
        .summary = "Defines document I/O security policy for trust, paths, permissions, limits, auditing and redaction.",
        .provider_role = "framework-core",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI | UMI_DOCUMENT_CAPABILITY_FLAG_SECURITY_SENSITIVE | UMI_DOCUMENT_CAPABILITY_FLAG_IMPLEMENTED,
        .priority = 780
    };
    return &descriptor;
}
