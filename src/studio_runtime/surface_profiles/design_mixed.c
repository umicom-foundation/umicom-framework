/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/surface_profiles/design_mixed.c
 * PURPOSE: Bind split code-and-design authoring to Studio's central work area.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/surface_profiles/design_mixed.h"

/* Keep placement metadata independent from the GTK presentation adapter. */
const UmiStudioRuntimeSurfaceBinding *
umi_studio_surface_profile_design_mixed(void)
{
    static const UmiStudioRuntimeSurfaceBinding binding = {
        .kind = UMI_STUDIO_SURFACE_DESIGN_MIXED,
        .ide_surface_id = "umicom.ide.design-mixed",
        .preferred_contribution_id = "umicom.shell.designer.mixed",
        .fallback_contribution_id = "umicom.studio.surface.design-mixed",
        .activity_id = "umicom.activity.visual-design",
        .container_id = "studio.editor",
        .title = "Mixed",
        .role = UMI_APPLICATION_SHELL_ROLE_EDITOR,
        .region = UMI_APPLICATION_SHELL_REGION_EDITOR,
        .order = 112,
        .default_visible = 0,
        .create_if_missing = 1
    };
    return &binding;
}
