/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/revert_command.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Revert Command document capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/revert_command.h"

/*
 * Provide the document capability revert command operation used by this module and its
 * client applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_revert_command(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_REVERT_COMMAND,
        .title = "Revert Command",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_COMMAND,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_PREVIEW,
        .summary = "Reloads a saved document through its provider; unsaved drafts require an explicit prepared-plan confirmation and remain recoverable through Undo.",
        .provider_role = "framework-core",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI | UMI_DOCUMENT_CAPABILITY_FLAG_IMPLEMENTED,
        .priority = 630
    };
    return &descriptor;
}
