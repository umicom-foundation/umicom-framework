/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/surface_profiles/test_explorer.c
 *
 * PURPOSE:
 *   Define the Test Explorer logical IDE surface without physical docking geometry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/surface_profiles/test_explorer.h"

/*
 * Provide the ide surface profile test explorer operation used by this module and its
 * client applications.
 */
const UmiIdeSurfaceDescriptor *umi_ide_surface_profile_test_explorer(void)
{
    static const UmiIdeSurfaceDescriptor descriptor = {
        .surface_id = "umicom.ide.test-explorer",
        .title = "Test Explorer",
        .description = "Test discovery, execution and results.",
        .command_id = "workbench.view.test-explorer",
        .required_capability = "testing",
        .default_region = UMI_IDE_SURFACE_REGION_PRIMARY_SIDEBAR,
        .default_visible = 0,
        .closable = 1,
        .movable = 1,
        .multi_instance = 0,
        .revision = 1U
    };
    return &descriptor;
}
