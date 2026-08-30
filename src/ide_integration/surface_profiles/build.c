/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/surface_profiles/build.c
 *
 * PURPOSE:
 *   Define the Build logical IDE surface without physical docking geometry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/surface_profiles/build.h"

const UmiIdeSurfaceDescriptor *umi_ide_surface_profile_build(void)
{
    static const UmiIdeSurfaceDescriptor descriptor = {
        .surface_id = "umicom.ide.build",
        .title = "Build",
        .description = "Build progress, history and artifacts.",
        .command_id = "workbench.view.build",
        .required_capability = "build",
        .default_region = UMI_IDE_SURFACE_REGION_BOTTOM_PANEL,
        .default_visible = 0,
        .closable = 1,
        .movable = 1,
        .multi_instance = 0,
        .revision = 1U
    };
    return &descriptor;
}
