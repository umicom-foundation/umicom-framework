/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_ui_workspace_profile.c
 *
 * PURPOSE:
 *   Verify named workspace profile registration, activation and replacement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/umicom.h"
#include "umicom/ui/internal/workspace_profile_owner.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Exercise profile and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiUiWorkspaceProfileSnapshot profile(const char *id, const char *label, int sidebar,
                                             int auxiliary, int bottom, int order) {
  UmiUiWorkspaceProfileSnapshot item = {0};
  (void)snprintf(item.profile_id, sizeof(item.profile_id), "%s", id);
  (void)snprintf(item.label, sizeof(item.label), "%s", label);
  item.sidebar_visible = sidebar;
  item.auxiliary_sidebar_visible = auxiliary;
  item.bottom_panel_visible = bottom;
  item.sidebar_size = 288;
  item.auxiliary_sidebar_size = 360;
  item.bottom_panel_size = 240;
  item.editor_split_mode = UMI_UI_EDITOR_SPLIT_SINGLE;
  item.editor_split_ratio = UMI_UI_EDITOR_SPLIT_RATIO_DEFAULT;
  item.order = order;
  item.built_in = 1;
  return item;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
  UmiUiWorkspaceProfileModel *model = NULL;
  UmiUiWorkspaceProfileSnapshot develop = profile("develop", "Develop", 1, 0, 1, 10);
  UmiUiWorkspaceProfileSnapshot focus = profile("focus", "Focus", 0, 0, 0, 20);
  UmiUiWorkspaceProfileSnapshot result;

  assert(umi_ui_workspace_profile_model_create(&model) == UMI_STATUS_OK);
  assert(umi_ui_workspace_profile_model_upsert(model, &develop) == UMI_STATUS_OK);
  assert(umi_ui_workspace_profile_model_upsert(model, &focus) == UMI_STATUS_OK);
  assert(umi_ui_workspace_profile_model_count(model) == 2U);
  assert(umi_ui_workspace_profile_model_set_active(model, "focus") == UMI_STATUS_OK);
  assert(umi_ui_workspace_profile_model_find(model, "focus", &result) == UMI_STATUS_OK);
  assert(result.active && !result.sidebar_visible && !result.bottom_panel_visible);
  assert(umi_ui_workspace_profile_model_find(model, "develop", &result) == UMI_STATUS_OK);
  assert(!result.active);
  /* Built-in profiles are immutable safety anchors. User-created profiles
   * can still be unlocked, renamed, updated and removed explicitly. */
  assert(umi_ui_workspace_profile_model_remove(model, "focus") == UMI_STATUS_PERMISSION_DENIED);
  assert(umi_ui_workspace_profile_model_upsert(model, &focus) == UMI_STATUS_PERMISSION_DENIED);
  (void)snprintf(focus.description, sizeof(focus.description), "%s",
                 "Refreshed by Framework composition");
  assert(umi_ui_workspace_profile_model_install_built_in(model, &focus) == UMI_STATUS_OK);
  assert(umi_ui_workspace_profile_model_find(model, "focus", &result) == UMI_STATUS_OK);
  assert(result.active);
  assert(strcmp(result.description, "Refreshed by Framework composition") == 0);
  result = profile("custom-review", "My Review", 1, 1, 1, 100);
  result.built_in = 0;
  result.locked = 0;
  result.active = 0;
  assert(umi_ui_workspace_profile_model_upsert(model, &result) == UMI_STATUS_OK);
  result.built_in = 1;
  assert(umi_ui_workspace_profile_model_install_built_in(model, &result) ==
         UMI_STATUS_PERMISSION_DENIED);
  result.built_in = 0;
  assert(umi_ui_workspace_profile_model_rename(model, "custom-review", "Review Layout",
                                               "Saved by user") == UMI_STATUS_OK);
  assert(umi_ui_workspace_profile_model_remove(model, "custom-review") == UMI_STATUS_OK);
  assert(umi_ui_workspace_profile_model_count(model) == 2U);

  umi_ui_workspace_profile_model_destroy(model);
  return EXIT_SUCCESS;
}
