/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/command_aliases/view_bookmarks.c
 *
 * PURPOSE:
 *   Define Studio compatibility alias view.bookmarks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/command_aliases/view_bookmarks.h"

/*
 * Provide the studio command alias view bookmark navigation operation used by this module and
 * its client applications.
 */
const UmiStudioRuntimeCommandAliasDefinition *umi_studio_command_alias_view_bookmarks(void) {
  static const UmiStudioRuntimeCommandAliasDefinition definition = {
      .alias_id = "view.bookmarks",
      .title = "Bookmarks",
      .category = "View",
      .description = "Open Bookmarks.",
      .target_command_id = "",
      .surface = UMI_STUDIO_SURFACE_BOOKMARKS,
      .activate_surface = 1,
      .mutates_state = 0};
  return &definition;
}
