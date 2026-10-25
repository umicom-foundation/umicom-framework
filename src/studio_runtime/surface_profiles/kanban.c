/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/surface_profiles/kanban.c
 *
 * PURPOSE:
 *   Define the Project Board Studio/Application Shell binding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/surface_profiles/kanban.h"

/*
 * Provide the studio surface profile project board operation used by this module and its
 * client applications.
 */
const UmiStudioRuntimeSurfaceBinding *umi_studio_surface_profile_kanban(void) {
  static const UmiStudioRuntimeSurfaceBinding binding = {
      .kind = UMI_STUDIO_SURFACE_KANBAN,
      .ide_surface_id = "umicom.ide.kanban",
      .preferred_contribution_id = "umicom.shell.kanban.kanban",
      .fallback_contribution_id = "umicom.studio.surface.kanban",
      .activity_id = "umicom.shell.activity-bar.planning",
      .container_id = "studio.bottom-panel",
      .title = "Project Board",
      .role = UMI_APPLICATION_SHELL_ROLE_PANEL,
      .region = UMI_APPLICATION_SHELL_REGION_BOTTOM_PANEL,
      .order = 190,
      .default_visible = 0,
      .create_if_missing = 1};
  return &binding;
}
