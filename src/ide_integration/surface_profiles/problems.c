/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/surface_profiles/problems.c
 *
 * PURPOSE:
 *   Define the Problems logical IDE surface without physical docking geometry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/surface_profiles/problems.h"

/*
 * Provide the ide surface profile problems operation used by this module and its client
 * applications.
 */
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
