/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/activation_profiles/kanban.c
 *
 * PURPOSE:
 *   Define Project Board command activation behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/activation_profiles/kanban.h"

static const UmiStudioRuntimeActivationRule RULES[] = {{.command_pattern = "view.kanban",
                                                        .surface = UMI_STUDIO_SURFACE_KANBAN,
                                                        .prefix_match = 0,
                                                        .make_visible = 1,
                                                        .focus = 1},
                                                       {.command_pattern = "workbench.view.kanban",
                                                        .surface = UMI_STUDIO_SURFACE_KANBAN,
                                                        .prefix_match = 0,
                                                        .make_visible = 1,
                                                        .focus = 1},
                                                       {.command_pattern = "project.board-pane",
                                                        .surface = UMI_STUDIO_SURFACE_KANBAN,
                                                        .prefix_match = 0,
                                                        .make_visible = 1,
                                                        .focus = 1}};

/*
 * Provide the studio activation profile project board operation used by this module and
 * its client applications.
 */
const UmiStudioRuntimeActivationProfile *umi_studio_activation_profile_kanban(void) {
  static const UmiStudioRuntimeActivationProfile profile = {
      .profile_id = "umicom.studio.activation.kanban",
      .rules = RULES,
      .rule_count = sizeof(RULES) / sizeof(RULES[0])};
  return &profile;
}
