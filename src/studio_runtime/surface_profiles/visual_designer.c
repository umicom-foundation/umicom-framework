/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/surface_profiles/visual_designer.c
 * PURPOSE: Bind the visual design canvas to Studio's central work area.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/surface_profiles/visual_designer.h"

/* Keep visual-canvas placement reusable by every supported frontend. */
const UmiStudioRuntimeSurfaceBinding *
umi_studio_surface_profile_visual_designer(void)
{
    static const UmiStudioRuntimeSurfaceBinding binding = {
        .kind = UMI_STUDIO_SURFACE_VISUAL_DESIGNER,
        .ide_surface_id = "umicom.ide.visual-designer",
        .preferred_contribution_id = "umicom.shell.designer.canvas",
        .fallback_contribution_id = "umicom.studio.surface.visual-designer",
        .activity_id = "umicom.activity.visual-design",
        .container_id = "studio.editor",
        .title = "Visual Designer",
        .role = UMI_APPLICATION_SHELL_ROLE_EDITOR,
        .region = UMI_APPLICATION_SHELL_REGION_EDITOR,
        .order = 110,
        .default_visible = 0,
        .create_if_missing = 1
    };
    return &binding;
}
