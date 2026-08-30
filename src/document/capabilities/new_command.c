/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/new_command.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the New Command document capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/new_command.h"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_new_command(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_NEW_COMMAND,
        .title = "New Command",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_COMMAND,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_STABLE,
        .summary = "Defines a canonical document command consumable by menus, keybindings, automation and headless clients.",
        .provider_role = "framework-core",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI | UMI_DOCUMENT_CAPABILITY_FLAG_IMPLEMENTED,
        .priority = 570
    };
    return &descriptor;
}
