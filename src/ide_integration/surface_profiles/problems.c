/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/surface_profiles/problems.c
 *
 * PURPOSE:
 *   Define the Problems logical IDE surface without physical docking geometry.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/surface_profiles/problems.h"

const UmiIdeSurfaceDescriptor *umi_ide_surface_profile_problems(void)
{
    static const UmiIdeSurfaceDescriptor descriptor = {
        .surface_id = "umicom.ide.problems",
        .title = "Problems",
        .description = "Compiler, build, test and language diagnostics.",
        .command_id = "workbench.view.problems",
        .required_capability = "problems",
        .default_region = UMI_IDE_SURFACE_REGION_BOTTOM_PANEL,
        .default_visible = 1,
        .closable = 1,
        .movable = 1,
        .multi_instance = 0,
        .revision = 1U
    };
    return &descriptor;
}
