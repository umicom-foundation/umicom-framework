/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/find_command.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Find Command document capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/find_command.h"

/*
 * Provide the document capability find command operation used by this module and its
 * client applications.
 */
const UmiDocumentCapabilityDescriptor *
umi_document_capability_find_command(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_FIND_COMMAND,
        .title = "Find Command",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_NAVIGATION,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_STABLE,
        .summary = "Defines reusable document search and navigation behaviour independent from a native editor widget.",
        .provider_role = "framework-core",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI | UMI_DOCUMENT_CAPABILITY_FLAG_IMPLEMENTED,
        .priority = 650
    };
    return &descriptor;
}
