/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/surface_profiles/editor.c
 *
 * PURPOSE:
 *   Define the Editor logical IDE surface without physical docking geometry.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/surface_profiles/editor.h"

const UmiIdeSurfaceDescriptor *umi_ide_surface_profile_editor(void)
{
    static const UmiIdeSurfaceDescriptor descriptor = {
        .surface_id = "umicom.ide.editor",
        .title = "Editor",
        .description = "Primary document and source editor area.",
        .command_id = "workbench.view.editor",
        .required_capability = "documents",
        .default_region = UMI_IDE_SURFACE_REGION_EDITOR,
        .default_visible = 1,
        .closable = 0,
        .movable = 1,
        .multi_instance = 1,
        .revision = 1U
    };
    return &descriptor;
}
