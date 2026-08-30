/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/surface_profiles/source_control.c
 *
 * PURPOSE:
 *   Define the Source Control logical IDE surface without physical docking geometry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/surface_profiles/source_control.h"

const UmiIdeSurfaceDescriptor *umi_ide_surface_profile_source_control(void)
{
    static const UmiIdeSurfaceDescriptor descriptor = {
        .surface_id = "umicom.ide.source-control",
        .title = "Source Control",
        .description = "Repository status, staging and changes.",
        .command_id = "workbench.view.source-control",
        .required_capability = "vcs",
        .default_region = UMI_IDE_SURFACE_REGION_PRIMARY_SIDEBAR,
        .default_visible = 0,
        .closable = 1,
        .movable = 1,
        .multi_instance = 0,
        .revision = 1U
    };
    return &descriptor;
}
