/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_suite_layout/test_resolved_profile.c
 * PURPOSE: Verify thin-product pane ID resolution preserves Framework layout semantics.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "umicom/application/experience_catalogue.h"
#include "umicom/application/suite_layout/profile_projection.h"
static const char *resolve(const UmiExperiencePanelDefinition *panel, void *data)
{
    static char id[UMI_UI_ID_CAPACITY];
    (void)data;
    (void)snprintf(id, sizeof(id), "thin.%s", panel->panel_id);
    return id;
}
int main(void)
{
    const UmiApplicationExperienceDefinition *experience =
        umi_application_experience_catalogue_find("org.umicom.studio");
    UmiUiWorkspaceProfileSnapshot profile;
    assert(experience != NULL);
    assert(umi_application_suite_layout_profile_project_resolved(
        experience, &experience->layouts[0], 1, resolve, NULL, &profile) ==
        UMI_STATUS_OK);
    assert(profile.pane_count > 0U);
    assert(strncmp(profile.panes[0].pane_id, "thin.", 5U) == 0);
    return 0;
}
