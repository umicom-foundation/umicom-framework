/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workspace_customisation_v2.c
 *
 * PURPOSE:
 *   Implement the test workspace customisation v2 behavior for
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
#include <string.h>
#include "umicom/ui/workspace_customisation_v2.h"
int main(void)
{
    UmiUiWorkspaceCustomisationV2 customisation;
    UmiUiWorkspaceCustomisationSnapshotV2 snapshot;
    UmiUiWorkspaceLayoutV3 develop;
    UmiUiWorkspaceLayoutV3 operations;
    UmiUiThemeProfileV2 theme;
    umi_ui_workspace_customisation_v2_init(&customisation);
    assert(umi_ui_workspace_layout_v3_init(&develop,"develop","Develop") == UMI_STATUS_OK);
    assert(umi_ui_workspace_layout_v3_init(&operations,"operations","Operations") == UMI_STATUS_OK);
    assert(umi_ui_workspace_customisation_v2_add_layout(&customisation,&develop) == UMI_STATUS_OK);
    assert(umi_ui_workspace_customisation_v2_add_layout(&customisation,&operations) == UMI_STATUS_OK);
    assert(umi_ui_workspace_customisation_v2_activate(&customisation,"operations") == UMI_STATUS_OK);
    assert(umi_ui_theme_profile_v2_init(&theme,"contrast","High Contrast",UMI_UI_THEME_MODE_HIGH_CONTRAST,UMI_UI_DENSITY_COMFORTABLE) == UMI_STATUS_OK);
    assert(umi_ui_workspace_customisation_v2_set_theme(&customisation,&theme) == UMI_STATUS_OK);
    umi_ui_workspace_customisation_v2_snapshot(&customisation,&snapshot);
    assert(snapshot.layouts == 2U);
    assert(strcmp(snapshot.active_layout_id,"operations") == 0);
    assert(strcmp(snapshot.theme_id,"contrast") == 0);
    return 0;
}
