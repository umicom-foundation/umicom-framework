/* Umicom Framework | Layout library and theme tests | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "umicom/ui/layout_library_v2.h"
#include "umicom/ui/theme_profile_v2.h"
int main(void)
{
    UmiUiLayoutLibraryV2 library = {0};
    UmiUiLayoutLibraryItemV2 preset = {0};
    UmiUiWorkspaceLayoutV3 layout;
    UmiUiThemeProfileV2 theme;
    char reason[192U];
    (void)snprintf(preset.preset_id,sizeof(preset.preset_id),"studio-develop");
    (void)snprintf(preset.category,sizeof(preset.category),"Development");
    assert(umi_ui_workspace_layout_v3_init(&preset.layout,"template","Template") == UMI_STATUS_OK);
    assert(umi_ui_layout_library_v2_add(&library,&preset) == UMI_STATUS_OK);
    assert(umi_ui_layout_library_v2_instantiate(&library,"studio-develop","develop","Develop",&layout) == UMI_STATUS_OK);
    assert(strcmp(layout.layout_id,"develop") == 0);
    assert(umi_ui_theme_profile_v2_init(&theme,"umicom-light","Umicom Light",UMI_UI_THEME_MODE_LIGHT,UMI_UI_DENSITY_COMPACT) == UMI_STATUS_OK);
    assert(umi_ui_theme_profile_v2_validate(&theme,reason,sizeof(reason)) == UMI_STATUS_OK);
    assert(strcmp(theme.background,"#f4f6fa") == 0);
    return 0;
}
