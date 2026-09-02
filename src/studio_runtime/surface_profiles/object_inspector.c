/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/surface_profiles/object_inspector.c
 * PURPOSE: Bind selected component properties to a movable Studio panel.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/surface_profiles/object_inspector.h"

/* Keep property-inspector placement reusable by every supported frontend. */
const UmiStudioRuntimeSurfaceBinding *
umi_studio_surface_profile_object_inspector(void)
{
    static const UmiStudioRuntimeSurfaceBinding binding = {
        .kind = UMI_STUDIO_SURFACE_OBJECT_INSPECTOR,
        .ide_surface_id = "umicom.ide.object-inspector",
        .preferred_contribution_id = "umicom.shell.designer.inspector",
        .fallback_contribution_id = "umicom.studio.surface.object-inspector",
        .activity_id = "umicom.activity.visual-design",
        .container_id = "studio.sidebar.secondary",
        .title = "Object Inspector",
        .role = UMI_APPLICATION_SHELL_ROLE_PANEL,
        .region = UMI_APPLICATION_SHELL_REGION_RIGHT,
        .order = 230,
        .default_visible = 0,
        .create_if_missing = 1
    };
    return &binding;
}
