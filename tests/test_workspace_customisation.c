/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workspace_customisation.c
 *
 * PURPOSE:
 *   Implement the test workspace customisation behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Workspace customisation tests | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/ui/workspace_customisation.h"
int main(void)
{
    UmiUiWorkspaceCustomisation *customisation;
    UmiUiWorkspaceCustomisationSnapshot snapshot;
    UmiUiWorkspaceLayout develop;
    UmiUiWorkspaceLayout operations;
    UmiUiThemeProfile theme;
    customisation = (UmiUiWorkspaceCustomisation *)malloc(sizeof(*customisation));
    assert(customisation != NULL);
    if (customisation == NULL) return 1;
    umi_ui_workspace_customisation_init(customisation);
    assert(umi_ui_workspace_layout_init(&develop,"develop","Develop") == UMI_STATUS_OK);
    assert(umi_ui_workspace_layout_init(&operations,"operations","Operations") == UMI_STATUS_OK);
    assert(umi_ui_workspace_customisation_add_layout(customisation,&develop) == UMI_STATUS_OK);
    assert(umi_ui_workspace_customisation_add_layout(customisation,&operations) == UMI_STATUS_OK);
    assert(umi_ui_workspace_customisation_activate(customisation,"operations") == UMI_STATUS_OK);
    assert(umi_ui_theme_profile_init(&theme,"contrast","High Contrast",UMI_UI_THEME_MODE_HIGH_CONTRAST,UMI_UI_DENSITY_COMFORTABLE) == UMI_STATUS_OK);
    assert(umi_ui_workspace_customisation_set_theme(customisation,&theme) == UMI_STATUS_OK);
    umi_ui_workspace_customisation_snapshot(customisation,&snapshot);
    assert(snapshot.layouts == 2U);
    assert(strcmp(snapshot.active_layout_id,"operations") == 0);
    assert(strcmp(snapshot.theme_id,"contrast") == 0);
    free(customisation);
    return 0;
}
