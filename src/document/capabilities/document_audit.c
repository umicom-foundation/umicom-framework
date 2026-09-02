/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/document_audit.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Document Audit document capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/document_audit.h"

/*
 * Provide the document capability document audit operation used by this module and its
 * client applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_document_audit(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_DOCUMENT_AUDIT,
        .title = "Document Audit",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_SECURITY,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_PLANNED,
        .summary = "Defines document I/O security policy for trust, paths, permissions, limits, auditing and redaction.",
        .provider_role = "framework-extension",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI | UMI_DOCUMENT_CAPABILITY_FLAG_SECURITY_SENSITIVE,
        .priority = 790
    };
    return &descriptor;
}
