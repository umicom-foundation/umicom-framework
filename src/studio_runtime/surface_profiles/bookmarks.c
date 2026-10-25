/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/surface_profiles/bookmarks.c
 *
 * PURPOSE:
 *   Define the Bookmarks Studio/Application Shell binding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/surface_profiles/bookmarks.h"

/*
 * Provide the studio surface profile bookmark navigation operation used by this module and its
 * client applications.
 */
const UmiStudioRuntimeSurfaceBinding *umi_studio_surface_profile_bookmarks(void) {
  static const UmiStudioRuntimeSurfaceBinding binding = {
      .kind = UMI_STUDIO_SURFACE_BOOKMARKS,
      .ide_surface_id = "umicom.ide.bookmarks",
      .preferred_contribution_id = "umicom.shell.bookmarks.bookmarks",
      .fallback_contribution_id = "umicom.studio.surface.bookmarks",
      .activity_id = "umicom.shell.activity-bar.navigation",
      .container_id = "studio.bottom-panel",
      .title = "Bookmarks",
      .role = UMI_APPLICATION_SHELL_ROLE_PANEL,
      .region = UMI_APPLICATION_SHELL_REGION_BOTTOM_PANEL,
      .order = 195,
      .default_visible = 0,
      .create_if_missing = 1};
  return &binding;
}
