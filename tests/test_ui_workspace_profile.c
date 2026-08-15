/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_ui_workspace_profile.c
 *
 * PURPOSE:
 *   Verify named workspace profile registration, activation and replacement.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/umicom.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static UmiUiWorkspaceProfileSnapshot profile(const char *id,
                                              const char *label,
                                              int sidebar,
                                              int auxiliary,
                                              int bottom,
                                              int order)
{
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

int main(void)
{
    UmiUiWorkspaceProfileModel *model = NULL;
    UmiUiWorkspaceProfileSnapshot develop =
        profile("develop", "Develop", 1, 0, 1, 10);
    UmiUiWorkspaceProfileSnapshot focus =
        profile("focus", "Focus", 0, 0, 0, 20);
    UmiUiWorkspaceProfileSnapshot result;

    assert(umi_ui_workspace_profile_model_create(&model) == UMI_STATUS_OK);
    assert(umi_ui_workspace_profile_model_upsert(model, &develop) ==
           UMI_STATUS_OK);
    assert(umi_ui_workspace_profile_model_upsert(model, &focus) ==
           UMI_STATUS_OK);
    assert(umi_ui_workspace_profile_model_count(model) == 2U);
    assert(umi_ui_workspace_profile_model_set_active(model, "focus") ==
           UMI_STATUS_OK);
    assert(umi_ui_workspace_profile_model_find(model, "focus", &result) ==
           UMI_STATUS_OK);
    assert(result.active && !result.sidebar_visible &&
           !result.bottom_panel_visible);
    assert(umi_ui_workspace_profile_model_find(model, "develop", &result) ==
           UMI_STATUS_OK);
    assert(!result.active);
    /* Built-in profiles are immutable safety anchors. User-created profiles
     * can still be unlocked, renamed, updated and removed explicitly. */
    assert(umi_ui_workspace_profile_model_remove(model, "focus") ==
           UMI_STATUS_PERMISSION_DENIED);
    assert(umi_ui_workspace_profile_model_upsert(model, &focus) ==
           UMI_STATUS_PERMISSION_DENIED);
    result = profile("custom-review", "My Review", 1, 1, 1, 100);
    result.built_in = 0;
    result.locked = 0;
    result.active = 0;
    assert(umi_ui_workspace_profile_model_upsert(model, &result) ==
           UMI_STATUS_OK);
    assert(umi_ui_workspace_profile_model_rename(
               model, "custom-review", "Review Layout", "Saved by user") ==
           UMI_STATUS_OK);
    assert(umi_ui_workspace_profile_model_remove(model, "custom-review") ==
           UMI_STATUS_OK);
    assert(umi_ui_workspace_profile_model_count(model) == 2U);

    umi_ui_workspace_profile_model_destroy(model);
    return EXIT_SUCCESS;
}
