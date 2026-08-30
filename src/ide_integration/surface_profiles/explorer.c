/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/surface_profiles/explorer.c
 *
 * PURPOSE:
 *   Define the Explorer logical IDE surface without physical docking geometry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/surface_profiles/explorer.h"

const UmiIdeSurfaceDescriptor *umi_ide_surface_profile_explorer(void)
{
    static const UmiIdeSurfaceDescriptor descriptor = {
        .surface_id = "umicom.ide.explorer",
        .title = "Explorer",
        .description = "Workspace/project file navigation.",
        .command_id = "workbench.view.explorer",
        .required_capability = "project",
        .default_region = UMI_IDE_SURFACE_REGION_PRIMARY_SIDEBAR,
        .default_visible = 1,
        .closable = 1,
        .movable = 1,
        .multi_instance = 0,
        .revision = 1U
    };
    return &descriptor;
}
