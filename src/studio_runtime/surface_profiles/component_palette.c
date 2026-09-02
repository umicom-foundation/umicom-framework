/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/surface_profiles/component_palette.c
 * PURPOSE: Bind the searchable component palette to a movable Studio panel.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/surface_profiles/component_palette.h"

/* Keep component-palette placement reusable by every supported frontend. */
const UmiStudioRuntimeSurfaceBinding *
umi_studio_surface_profile_component_palette(void)
{
    static const UmiStudioRuntimeSurfaceBinding binding = {
        .kind = UMI_STUDIO_SURFACE_COMPONENT_PALETTE,
        .ide_surface_id = "umicom.ide.component-palette",
        .preferred_contribution_id = "umicom.shell.designer.palette",
        .fallback_contribution_id = "umicom.studio.surface.component-palette",
        .activity_id = "umicom.activity.visual-design",
        .container_id = "studio.sidebar.primary",
        .title = "Components",
        .role = UMI_APPLICATION_SHELL_ROLE_PANEL,
        /* The component list belongs in the shell's main navigation side. */
        .region = UMI_APPLICATION_SHELL_REGION_PRIMARY_SIDEBAR,
        .order = 210,
        .default_visible = 0,
        .create_if_missing = 1
    };
    return &binding;
}
