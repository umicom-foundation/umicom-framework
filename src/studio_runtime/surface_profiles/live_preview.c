/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/surface_profiles/live_preview.c
 *
 * PURPOSE:
 *   Bind active-document preview to a movable Studio tool region.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/surface_profiles/live_preview.h"

/* Keep semantic placement independent from any native frontend toolkit. */
const UmiStudioRuntimeSurfaceBinding *
umi_studio_surface_profile_live_preview(void)
{
    static const UmiStudioRuntimeSurfaceBinding binding = {
        .kind = UMI_STUDIO_SURFACE_LIVE_PREVIEW,
        .ide_surface_id = "umicom.ide.live-preview",
        .preferred_contribution_id = "umicom.shell.editor.live-preview",
        .fallback_contribution_id = "umicom.studio.surface.live-preview",
        .activity_id = "umicom.activity.development",
        .container_id = "studio.secondary",
        .title = "Live Preview",
        .role = UMI_APPLICATION_SHELL_ROLE_PANEL,
        /* Preview tools sit opposite navigation in the secondary sidebar. */
        .region = UMI_APPLICATION_SHELL_REGION_SECONDARY_SIDEBAR,
        .order = 114,
        .default_visible = 0,
        .create_if_missing = 1
    };
    return &binding;
}
