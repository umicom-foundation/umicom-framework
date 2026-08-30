/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/command_aliases/document_save.c
 *
 * PURPOSE:
 *   Define Studio compatibility alias document.save.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/command_aliases/document_save.h"

const UmiStudioRuntimeCommandAliasDefinition *
umi_studio_command_alias_document_save(void)
{
    static const UmiStudioRuntimeCommandAliasDefinition definition = {
        .alias_id = "document.save",
        .title = "Save Document",
        .category = "File",
        .description = "Save the active document.",
        .target_command_id = "ide.document.save",
        .surface = UMI_STUDIO_SURFACE_EDITOR,
        .activate_surface = 0,
        .mutates_state = 1
    };
    return &definition;
}
