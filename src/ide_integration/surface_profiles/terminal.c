/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/surface_profiles/terminal.c
 *
 * PURPOSE:
 *   Define the Terminal logical IDE surface without physical docking geometry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/surface_profiles/terminal.h"

const UmiIdeSurfaceDescriptor *umi_ide_surface_profile_terminal(void)
{
    static const UmiIdeSurfaceDescriptor descriptor = {
        .surface_id = "umicom.ide.terminal",
        .title = "Terminal",
        .description = "Integrated terminal sessions.",
        .command_id = "workbench.view.terminal",
        .required_capability = "terminal",
        .default_region = UMI_IDE_SURFACE_REGION_BOTTOM_PANEL,
        .default_visible = 0,
        .closable = 1,
        .movable = 1,
        .multi_instance = 1,
        .revision = 1U
    };
    return &descriptor;
}
