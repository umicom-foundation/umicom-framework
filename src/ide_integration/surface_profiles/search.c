/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/surface_profiles/search.c
 *
 * PURPOSE:
 *   Define the Search logical IDE surface without physical docking geometry.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/surface_profiles/search.h"

const UmiIdeSurfaceDescriptor *umi_ide_surface_profile_search(void)
{
    static const UmiIdeSurfaceDescriptor descriptor = {
        .surface_id = "umicom.ide.search",
        .title = "Search",
        .description = "Workspace text and symbol search.",
        .command_id = "workbench.view.search",
        .required_capability = "search",
        .default_region = UMI_IDE_SURFACE_REGION_PRIMARY_SIDEBAR,
        .default_visible = 0,
        .closable = 1,
        .movable = 1,
        .multi_instance = 0,
        .revision = 1U
    };
    return &descriptor;
}
