/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/command_aliases/view_kanban.c
 *
 * PURPOSE:
 *   Define Studio compatibility alias view.kanban.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/command_aliases/view_kanban.h"

/*
 * Provide the studio command alias view project board operation used by this module and
 * its client applications.
 */
const UmiStudioRuntimeCommandAliasDefinition *umi_studio_command_alias_view_kanban(void) {
  static const UmiStudioRuntimeCommandAliasDefinition definition = {
      .alias_id = "view.kanban",
      .title = "Project Board",
      .category = "View",
      .description = "Open Project Board.",
      .target_command_id = "",
      .surface = UMI_STUDIO_SURFACE_KANBAN,
      .activate_surface = 1,
      .mutates_state = 0};
  return &definition;
}
