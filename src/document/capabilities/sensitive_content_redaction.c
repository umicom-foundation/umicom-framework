/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/sensitive_content_redaction.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Sensitive Content Redaction document capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/sensitive_content_redaction.h"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_sensitive_content_redaction(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_SENSITIVE_CONTENT_REDACTION,
        .title = "Sensitive Content Redaction",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_SECURITY,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_PLANNED,
        .summary = "Defines document I/O security policy for trust, paths, permissions, limits, auditing and redaction.",
        .provider_role = "framework-extension",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI | UMI_DOCUMENT_CAPABILITY_FLAG_SECURITY_SENSITIVE,
        .priority = 800
    };
    return &descriptor;
}
