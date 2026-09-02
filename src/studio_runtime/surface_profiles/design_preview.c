/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/surface_profiles/design_preview.c
 * PURPOSE: Bind revision-aware live preview to Studio's central work area.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/surface_profiles/design_preview.h"

/* Keep placement metadata independent from the GTK presentation adapter. */
const UmiStudioRuntimeSurfaceBinding *
umi_studio_surface_profile_design_preview(void)
{
    static const UmiStudioRuntimeSurfaceBinding binding = {
        .kind = UMI_STUDIO_SURFACE_DESIGN_PREVIEW,
        .ide_surface_id = "umicom.ide.design-preview",
        .preferred_contribution_id = "umicom.shell.designer.preview",
        .fallback_contribution_id = "umicom.studio.surface.design-preview",
        .activity_id = "umicom.activity.visual-design",
        .container_id = "studio.editor",
        .title = "Preview",
        .role = UMI_APPLICATION_SHELL_ROLE_EDITOR,
        .region = UMI_APPLICATION_SHELL_REGION_EDITOR,
        .order = 113,
        .default_visible = 0,
        .create_if_missing = 1
    };
    return &binding;
}
