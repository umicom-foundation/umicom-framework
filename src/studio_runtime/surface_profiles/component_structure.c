/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/surface_profiles/component_structure.c
 * PURPOSE: Bind the semantic component hierarchy to a movable Studio panel.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/surface_profiles/component_structure.h"

/* Keep component-tree placement reusable by every supported frontend. */
const UmiStudioRuntimeSurfaceBinding *
umi_studio_surface_profile_component_structure(void)
{
    static const UmiStudioRuntimeSurfaceBinding binding = {
        .kind = UMI_STUDIO_SURFACE_COMPONENT_STRUCTURE,
        .ide_surface_id = "umicom.ide.component-structure",
        .preferred_contribution_id = "umicom.shell.designer.structure",
        .fallback_contribution_id = "umicom.studio.surface.component-structure",
        .activity_id = "umicom.activity.visual-design",
        .container_id = "studio.sidebar.primary",
        .title = "Structure",
        .role = UMI_APPLICATION_SHELL_ROLE_PANEL,
        .region = UMI_APPLICATION_SHELL_REGION_LEFT,
        .order = 220,
        .default_visible = 0,
        .create_if_missing = 1
    };
    return &binding;
}
