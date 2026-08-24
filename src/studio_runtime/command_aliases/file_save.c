/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/command_aliases/file_save.c
 *
 * PURPOSE:
 *   Define Studio compatibility alias file.save.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/command_aliases/file_save.h"

const UmiStudioRuntimeCommandAliasDefinition *
umi_studio_command_alias_file_save(void)
{
    static const UmiStudioRuntimeCommandAliasDefinition definition = {
        .alias_id = "file.save",
        .title = "Save",
        .category = "File",
        .description = "Save the active document.",
        .target_command_id = "ide.document.save",
        .surface = UMI_STUDIO_SURFACE_EDITOR,
        .activate_surface = 0,
        .mutates_state = 1
    };
    return &definition;
}
