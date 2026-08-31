/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workspace_customisation_suite.c
 *
 * PURPOSE:
 *   Verify every Umicom application can load Framework-owned layouts and use
 *   guarded docking, floating, auto-hide and panel-closing operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/application/experience_catalogue.h"
#include "umicom/application/suite_layout/customisation.h"
#include "umicom/ui/workspace_customisation.h"

/* Locate a visible layout instance which advertises a requested behaviour. */
static const UmiUiWorkspaceWindow *find_window_with_flag(
    const UmiUiWorkspaceCustomisation *customisation,
    const UmiApplicationExperienceDefinition *experience,
    uint32_t flag)
{
    const UmiUiWorkspaceLayout *layout =
        umi_ui_workspace_customisation_active_const(customisation);
    size_t index;

    if (layout == NULL || experience == NULL) {
        return NULL;
    }
    for (index = 0U; index < layout->window_count; ++index) {
        const UmiExperiencePanelDefinition *panel =
            umi_application_experience_panel_find(
                experience, layout->windows[index].tool_id);
        if (panel != NULL && (panel->flags & flag) != 0U) {
            return &layout->windows[index];
        }
    }
    return NULL;
}

int main(void)
{
    UmiUiWorkspaceCustomisation *customisation =
        (UmiUiWorkspaceCustomisation *)calloc(1U, sizeof(*customisation));
    size_t index;

    assert(customisation != NULL);
    if (customisation == NULL) {
        return 1;
    }

    /* Every current and future thin application uses the same loading path. */
    for (index = 0U;
         index < umi_application_experience_catalogue_count();
         ++index) {
        const UmiApplicationExperienceDefinition *experience =
            umi_application_experience_catalogue_at(index);
        UmiUiWorkspaceCustomisationSnapshot snapshot;

        assert(experience != NULL);
        assert(umi_application_suite_customisation_load_experience(
                   customisation, experience) == UMI_STATUS_OK);
        umi_ui_workspace_customisation_snapshot(customisation, &snapshot);
        assert(snapshot.layouts == experience->layout_count);
        assert(snapshot.available_windows == experience->panel_count);
        assert(snapshot.active_layout_locked);
        assert(!snapshot.editing);
    }

    {
        const UmiApplicationExperienceDefinition *studio =
            umi_application_experience_catalogue_find("org.umicom.studio");
        const UmiUiWorkspaceWindow *floating;
        const UmiUiWorkspaceWindow *auto_hide;
        char floating_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
        char auto_hide_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];

        assert(studio != NULL);
        assert(umi_application_suite_customisation_load_experience(
                   customisation, studio) == UMI_STATUS_OK);
        floating = find_window_with_flag(
            customisation, studio, UMI_EXPERIENCE_PANEL_FLOATABLE);
        auto_hide = find_window_with_flag(
            customisation, studio, UMI_EXPERIENCE_PANEL_AUTO_HIDE);
        assert(floating != NULL);
        assert(auto_hide != NULL);
        assert(snprintf(
                   floating_id,
                   sizeof(floating_id),
                   "%s",
                   floating->window_id) > 0);
        assert(snprintf(
                   auto_hide_id,
                   sizeof(auto_hide_id),
                   "%s",
                   auto_hide->window_id) > 0);

        /* One edit session can detach, redock and collapse shared panels. */
        assert(umi_ui_workspace_customisation_begin_edit(customisation) ==
               UMI_STATUS_OK);
        assert(umi_application_suite_customisation_float_window(
                   customisation,
                   floating_id,
                   0.20,
                   0.15,
                   0.55,
                   0.60) == UMI_STATUS_OK);
        assert(umi_application_suite_customisation_dock_window(
                   customisation,
                   floating_id,
                   "right",
                   "details") == UMI_STATUS_OK);
        assert(umi_application_suite_customisation_set_auto_hidden(
                   customisation, auto_hide_id, true) == UMI_STATUS_OK);
        assert(umi_ui_workspace_customisation_window_is_auto_hidden(
            customisation, auto_hide_id));
        assert(umi_application_suite_customisation_set_auto_hidden(
                   customisation, auto_hide_id, false) == UMI_STATUS_OK);
        assert(!umi_ui_workspace_customisation_window_is_auto_hidden(
            customisation, auto_hide_id));
        assert(umi_ui_workspace_customisation_commit_edit(customisation) ==
               UMI_STATUS_OK);
    }

    {
        const UmiApplicationExperienceDefinition *trader =
            umi_application_experience_catalogue_find("org.umicom.trader");
        const UmiUiWorkspaceWindow *critical;

        assert(trader != NULL);
        assert(umi_application_suite_customisation_load_experience(
                   customisation, trader) == UMI_STATUS_OK);
        critical = find_window_with_flag(
            customisation, trader, UMI_EXPERIENCE_PANEL_CRITICAL);
        assert(critical != NULL);
        assert(umi_ui_workspace_customisation_begin_edit(customisation) ==
               UMI_STATUS_OK);
        /* Critical operational panels cannot be closed accidentally. */
        assert(umi_ui_workspace_customisation_close_window(
                   customisation, critical->window_id) ==
               UMI_STATUS_PERMISSION_DENIED);
        assert(umi_ui_workspace_customisation_cancel_edit(customisation) ==
               UMI_STATUS_OK);
    }

    free(customisation);
    return 0;
}
