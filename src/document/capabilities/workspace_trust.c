/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/workspace_trust.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Workspace Trust document capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/workspace_trust.h"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_workspace_trust(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_WORKSPACE_TRUST,
        .title = "Workspace Trust",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_SECURITY,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_PLANNED,
        .summary = "Defines document I/O security policy for trust, paths, permissions, limits, auditing and redaction.",
        .provider_role = "framework-extension",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI | UMI_DOCUMENT_CAPABILITY_FLAG_SECURITY_SENSITIVE,
        .priority = 730
    };
    return &descriptor;
}
