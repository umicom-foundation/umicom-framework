/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/surface_profiles/editor.c
 *
 * PURPOSE:
 *   Define the Code Editor Studio/Application Shell binding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/surface_profiles/editor.h"

const UmiStudioRuntimeSurfaceBinding *umi_studio_surface_profile_editor(void)
{
    static const UmiStudioRuntimeSurfaceBinding binding = {
        .kind = UMI_STUDIO_SURFACE_EDITOR,
        .ide_surface_id = "umicom.ide.editor",
        .preferred_contribution_id = "umicom.shell.editor.code-editor",
        .fallback_contribution_id = "umicom.studio.surface.editor",
        .activity_id = "",
        .container_id = "studio.editor",
        .title = "Code Editor",
        .role = UMI_APPLICATION_SHELL_ROLE_EDITOR,
        .region = UMI_APPLICATION_SHELL_REGION_EDITOR,
        .order = 100,
        .default_visible = 1,
        .create_if_missing = 1
    };
    return &binding;
}
