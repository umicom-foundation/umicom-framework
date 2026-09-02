/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/final_newline_policy.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Final Newline Policy document capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/final_newline_policy.h"

/*
 * Provide the document capability final newline policy operation used by this module and
 * its client applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_final_newline_policy(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_FINAL_NEWLINE_POLICY,
        .title = "Final Newline Policy",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_PERSISTENCE,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_PLANNED,
        .summary = "Defines safe persistence policy including atomic replacement and user-visible save behaviour.",
        .provider_role = "framework-extension",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI | UMI_DOCUMENT_CAPABILITY_FLAG_SECURITY_SENSITIVE | UMI_DOCUMENT_CAPABILITY_FLAG_PERSISTED,
        .priority = 390
    };
    return &descriptor;
}
