/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/surface_profiles/debug.c
 *
 * PURPOSE:
 *   Define the Run and Debug logical IDE surface without physical docking geometry.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/surface_profiles/debug.h"

const UmiIdeSurfaceDescriptor *umi_ide_surface_profile_debug(void)
{
    static const UmiIdeSurfaceDescriptor descriptor = {
        .surface_id = "umicom.ide.debug",
        .title = "Run and Debug",
        .description = "Debugger sessions, threads, stack and variables.",
        .command_id = "workbench.view.debug",
        .required_capability = "debug",
        .default_region = UMI_IDE_SURFACE_REGION_PRIMARY_SIDEBAR,
        .default_visible = 0,
        .closable = 1,
        .movable = 1,
        .multi_instance = 0,
        .revision = 1U
    };
    return &descriptor;
}
