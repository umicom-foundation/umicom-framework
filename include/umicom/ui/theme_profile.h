/* Umicom Framework | Theme profile v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_UI_THEME_PROFILE_H
#define UMICOM_UI_THEME_PROFILE_H
#include "umicom/ui/workspace_layout.h"
typedef enum UmiUiThemeMode { UMI_UI_THEME_MODE_SYSTEM = 1, UMI_UI_THEME_MODE_LIGHT, UMI_UI_THEME_MODE_DARK, UMI_UI_THEME_MODE_HIGH_CONTRAST } UmiUiThemeMode;
typedef enum UmiUiDensity { UMI_UI_DENSITY_COMPACT = 1, UMI_UI_DENSITY_COMFORTABLE, UMI_UI_DENSITY_SPACIOUS } UmiUiDensity;
typedef struct UmiUiThemeProfile { char theme_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY]; char name[UMI_UI_WORKSPACE_LAYOUT_NAME_CAPACITY]; UmiUiThemeMode mode; UmiUiDensity density; char accent[32U]; char background[32U]; char surface[32U]; char foreground[32U]; char success[32U]; char warning[32U]; char danger[32U]; double font_scale; uint64_t revision; } UmiUiThemeProfile;
UmiStatus umi_ui_theme_profile_init(UmiUiThemeProfile *theme,const char *theme_id,const char *name,UmiUiThemeMode mode,UmiUiDensity density);
UmiStatus umi_ui_theme_profile_validate(const UmiUiThemeProfile *theme,char *out_reason,size_t capacity);
#endif
