/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_layout_library_theme.c
 *
 * PURPOSE:
 *   Implement the test layout library theme behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Layout library and theme tests | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "umicom/ui/layout_library.h"
#include "umicom/ui/theme_profile.h"
int main(void)
{
    UmiUiLayoutLibrary *library = NULL;
    UmiUiLayoutLibraryItem preset = {0};
    UmiUiWorkspaceLayout layout;
    UmiUiThemeProfile theme;
    char reason[192U];
    assert(umi_ui_layout_library_create(&library) == UMI_STATUS_OK);
    (void)snprintf(preset.preset_id,sizeof(preset.preset_id),"studio-develop");
    (void)snprintf(preset.category,sizeof(preset.category),"Development");
    assert(umi_ui_workspace_layout_init(&preset.layout,"template","Template") == UMI_STATUS_OK);
    assert(umi_ui_layout_library_add(library,&preset) == UMI_STATUS_OK);
    assert(umi_ui_layout_library_instantiate(library,"studio-develop","develop","Develop",&layout) == UMI_STATUS_OK);
    assert(strcmp(layout.layout_id,"develop") == 0);
    assert(umi_ui_theme_profile_init(&theme,"umicom-light","Umicom Light",UMI_UI_THEME_MODE_LIGHT,UMI_UI_DENSITY_COMPACT) == UMI_STATUS_OK);
    assert(umi_ui_theme_profile_validate(&theme,reason,sizeof(reason)) == UMI_STATUS_OK);
    /* Light themes use the same canonical background as appearance profiles. */
    assert(strcmp(theme.background,"#F4F7FA") == 0);
    umi_ui_layout_library_destroy(library);
    return 0;
}
