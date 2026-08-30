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

const UmiDocumentCapabilityDescriptor *
umi_document_capability_revert_command(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_REVERT_COMMAND,
        .title = "Revert Command",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_COMMAND,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_PLANNED,
        .summary = "Defines a canonical document command consumable by menus, keybindings, automation and headless clients.",
        .provider_role = "framework-extension",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI,
        .priority = 630
    };
    return &descriptor;
}
