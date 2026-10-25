/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/activation_profiles/bookmarks.c
 *
 * PURPOSE:
 *   Define Bookmarks command activation behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/activation_profiles/bookmarks.h"

static const UmiStudioRuntimeActivationRule RULES[] = {
    {.command_pattern = "view.bookmarks",
     .surface = UMI_STUDIO_SURFACE_BOOKMARKS,
     .prefix_match = 0,
     .make_visible = 1,
     .focus = 1},
    {.command_pattern = "workbench.view.bookmarks",
     .surface = UMI_STUDIO_SURFACE_BOOKMARKS,
     .prefix_match = 0,
     .make_visible = 1,
     .focus = 1},
    {.command_pattern = "editor.bookmark-list",
     .surface = UMI_STUDIO_SURFACE_BOOKMARKS,
     .prefix_match = 0,
     .make_visible = 1,
     .focus = 1}};

/*
 * Provide the studio activation profile bookmark navigation operation used by this module and
 * its client applications.
 */
const UmiStudioRuntimeActivationProfile *umi_studio_activation_profile_bookmarks(void) {
  static const UmiStudioRuntimeActivationProfile profile = {
      .profile_id = "umicom.studio.activation.bookmarks",
      .rules = RULES,
      .rule_count = sizeof(RULES) / sizeof(RULES[0])};
  return &profile;
}
