/* Umicom Framework | Theme profile v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_UI_THEME_PROFILE_V2_H
#define UMICOM_UI_THEME_PROFILE_V2_H
#include "umicom/ui/workspace_layout_v3.h"
typedef enum UmiUiThemeModeV2 { UMI_UI_THEME_MODE_SYSTEM = 1, UMI_UI_THEME_MODE_LIGHT, UMI_UI_THEME_MODE_DARK, UMI_UI_THEME_MODE_HIGH_CONTRAST } UmiUiThemeModeV2;
typedef enum UmiUiDensityV2 { UMI_UI_DENSITY_COMPACT = 1, UMI_UI_DENSITY_COMFORTABLE, UMI_UI_DENSITY_SPACIOUS } UmiUiDensityV2;
typedef struct UmiUiThemeProfileV2 { char theme_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY]; char name[UMI_UI_WORKSPACE_LAYOUT_NAME_CAPACITY]; UmiUiThemeModeV2 mode; UmiUiDensityV2 density; char accent[32U]; char background[32U]; char surface[32U]; char foreground[32U]; char success[32U]; char warning[32U]; char danger[32U]; double font_scale; uint64_t revision; } UmiUiThemeProfileV2;
UmiStatus umi_ui_theme_profile_v2_init(UmiUiThemeProfileV2 *theme,const char *theme_id,const char *name,UmiUiThemeModeV2 mode,UmiUiDensityV2 density);
UmiStatus umi_ui_theme_profile_v2_validate(const UmiUiThemeProfileV2 *theme,char *out_reason,size_t capacity);
#endif
