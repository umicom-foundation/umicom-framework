/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capabilities/save_command.c
 *
 * PURPOSE:
 *   Define authoritative metadata for the Save Command document capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities/save_command.h"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_save_command(void)
{
    static const UmiDocumentCapabilityDescriptor descriptor = {
        .struct_size = sizeof(UmiDocumentCapabilityDescriptor),
        .api_version = UMI_DOCUMENT_CAPABILITY_API_VERSION,
        .capability_id = UMI_DOCUMENT_CAPABILITY_SAVE_COMMAND,
        .title = "Save Command",
        .category = UMI_DOCUMENT_CAPABILITY_CATEGORY_COMMAND,
        .maturity = UMI_DOCUMENT_CAPABILITY_MATURITY_STABLE,
        .summary = "Defines a canonical document command consumable by menus, keybindings, automation and headless clients.",
        .provider_role = "framework-core",
        .flags = UMI_DOCUMENT_CAPABILITY_FLAG_HEADLESS | UMI_DOCUMENT_CAPABILITY_FLAG_GUI | UMI_DOCUMENT_CAPABILITY_FLAG_IMPLEMENTED,
        .priority = 590
    };
    return &descriptor;
}
