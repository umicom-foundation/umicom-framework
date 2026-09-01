/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workspace_customisation_edit_session.c
 *
 * PURPOSE:
 *   Verify a layout can be unlocked as one guarded edit session, populated
 *   from the New Window catalogue, saved and later cancelled without losing
 *   the last locked arrangement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/ui/workspace_customisation.h"

int main(void)
{
    UmiUiWorkspaceCustomisation *customisation =
        (UmiUiWorkspaceCustomisation *)malloc(sizeof(*customisation));
    UmiUiWindowDescriptor terminal = {
        "terminal", "Terminal", "Run commands", "terminal",
        UMI_UI_WINDOW_CATEGORY_DEVELOPMENT, true, 0.50, 0.28
    };
    UmiUiWorkspaceLayout layout;
    UmiUiWorkspaceCustomisationSnapshot snapshot;
    char first_window[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char second_window[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];

    assert(customisation != NULL);
    if (customisation == NULL) return 1;
    umi_ui_workspace_customisation_init(customisation);
    assert(umi_ui_window_catalogue_register(
               &customisation->windows, &terminal) == UMI_STATUS_OK);
    assert(umi_ui_window_group_define(
               &customisation->groups,
               "project-blue",
               "blue",
               UMI_UI_WINDOW_CONTEXT_PROJECT) == UMI_STATUS_OK);
    /* A second group lets the test prove that Cancel restores both the panel's
     * text field and the reverse routing membership held by the group store. */
    assert(umi_ui_window_group_define(
               &customisation->groups,
               "project-orange",
               "orange",
               UMI_UI_WINDOW_CONTEXT_PROJECT) == UMI_STATUS_OK);
    assert(umi_ui_workspace_layout_init(&layout, "develop", "Develop") ==
           UMI_STATUS_OK);
    assert(umi_ui_workspace_customisation_add_layout(
               customisation, &layout) == UMI_STATUS_OK);

    assert(umi_ui_workspace_customisation_begin_edit(customisation) ==
           UMI_STATUS_OK);
    assert(umi_ui_workspace_customisation_open_window(
               customisation,
               "terminal",
               "bottom",
               false,
               UINT64_C(1000),
               first_window,
               sizeof(first_window)) == UMI_STATUS_OK);
    assert(strcmp(first_window, "terminal") == 0);
    assert(umi_ui_workspace_customisation_assign_context_group(
               customisation,
               first_window,
               "project-blue",
               UMI_UI_WINDOW_GROUP_BIDIRECTIONAL) == UMI_STATUS_OK);
    assert(strcmp(
               umi_ui_workspace_customisation_active(customisation)
                   ->windows[0].context_group_id,
               "project-blue") == 0);
    assert(umi_ui_workspace_customisation_commit_edit(customisation) ==
           UMI_STATUS_OK);
    assert(umi_ui_workspace_customisation_active(customisation)->locked);
    assert(umi_ui_workspace_customisation_active(customisation)->window_count ==
           1U);

    assert(umi_ui_workspace_customisation_begin_edit(customisation) ==
           UMI_STATUS_OK);
    assert(umi_ui_workspace_customisation_open_window(
               customisation,
               "terminal",
               "right",
               false,
               UINT64_C(2000),
               second_window,
               sizeof(second_window)) == UMI_STATUS_OK);
    assert(strcmp(second_window, "terminal-2") == 0);
    assert(umi_ui_workspace_customisation_assign_context_group(
               customisation,
               first_window,
               "project-orange",
               UMI_UI_WINDOW_GROUP_SOURCE) == UMI_STATUS_OK);
    assert(umi_ui_workspace_customisation_cancel_edit(customisation) ==
           UMI_STATUS_OK);
    assert(umi_ui_workspace_customisation_active(customisation)->window_count ==
           1U);
    assert(umi_ui_workspace_customisation_active(customisation)->locked);
    {
        UmiUiWindowGroupRole restored_role = UMI_UI_WINDOW_GROUP_SOURCE;
        const UmiUiWindowGroup *restored_group = umi_ui_window_group_for_window(
            &customisation->groups, first_window, &restored_role);

        /* Cancel must restore the original blue bidirectional relationship,
         * rather than leaving the abandoned orange route active. */
        assert(restored_group != NULL);
        assert(strcmp(restored_group->group_id, "project-blue") == 0);
        assert(restored_role == UMI_UI_WINDOW_GROUP_BIDIRECTIONAL);
    }

    umi_ui_workspace_customisation_snapshot(customisation, &snapshot);
    assert(!snapshot.editing);
    assert(snapshot.recent_windows == 1U);
    free(customisation);
    return 0;
}
