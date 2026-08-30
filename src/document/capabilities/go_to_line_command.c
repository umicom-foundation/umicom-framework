/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/go_to_line_command.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Go To Line Command document capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/go_to_line_command.h"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_go_to_line_command(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_GO_TO_LINE_COMMAND,
        .title = "Go To Line Command",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_NAVIGATION,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_STABLE,
        .summary = "Defines reusable document search and navigation behaviour independent from a native editor widget.",
        .provider_role = "framework-core",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI | UMI_DOCUMENT_CAPABILITY_FLAG_IMPLEMENTED,
        .priority = 670
    };
    return &descriptor;
}
