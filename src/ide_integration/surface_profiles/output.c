/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/surface_profiles/output.c
 *
 * PURPOSE:
 *   Define the Output logical IDE surface without physical docking geometry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/surface_profiles/output.h"

const UmiIdeSurfaceDescriptor *umi_ide_surface_profile_output(void)
{
    static const UmiIdeSurfaceDescriptor descriptor = {
        .surface_id = "umicom.ide.output",
        .title = "Output",
        .description = "Build, run and task output channels.",
        .command_id = "workbench.view.output",
        .required_capability = "output",
        .default_region = UMI_IDE_SURFACE_REGION_BOTTOM_PANEL,
        .default_visible = 1,
        .closable = 1,
        .movable = 1,
        .multi_instance = 0,
        .revision = 1U
    };
    return &descriptor;
}
