/* Umicom Framework | Theme profile v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/ui/theme_profile_v2.h"
#include <stdio.h>
#include <string.h>

static void palette(UmiUiThemeProfileV2 *theme)
{
    const bool light = theme->mode == UMI_UI_THEME_MODE_LIGHT;
    const bool contrast = theme->mode == UMI_UI_THEME_MODE_HIGH_CONTRAST;
    (void)snprintf(theme->accent,sizeof(theme->accent),"%s",contrast ? "#ffff00" : (light ? "#72161a" : "#c84c55"));
    (void)snprintf(theme->background,sizeof(theme->background),"%s",contrast ? "#000000" : (light ? "#eef1f4" : "#151b21"));
    (void)snprintf(theme->surface,sizeof(theme->surface),"%s",contrast ? "#000000" : (light ? "#ffffff" : "#1d2731"));
    (void)snprintf(theme->foreground,sizeof(theme->foreground),"%s",contrast ? "#ffffff" : (light ? "#243342" : "#e8edf2"));
    (void)snprintf(theme->success,sizeof(theme->success),"%s",contrast ? "#00ff80" : (light ? "#237a45" : "#54c98a"));
    (void)snprintf(theme->warning,sizeof(theme->warning),"%s",contrast ? "#ffff00" : (light ? "#8b5e0b" : "#e7b85c"));
    (void)snprintf(theme->danger,sizeof(theme->danger),"%s",contrast ? "#ff5f5f" : (light ? "#a51d28" : "#f07878"));
}
UmiStatus umi_ui_theme_profile_v2_init(UmiUiThemeProfileV2 *theme,const char *theme_id,const char *name,UmiUiThemeModeV2 mode,UmiUiDensityV2 density)
{
    int first; int second;
    if (theme == NULL || theme_id == NULL || name == NULL || theme_id[0] == '\0' || name[0] == '\0' || mode < UMI_UI_THEME_MODE_SYSTEM || mode > UMI_UI_THEME_MODE_HIGH_CONTRAST || density < UMI_UI_DENSITY_COMPACT || density > UMI_UI_DENSITY_SPACIOUS) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(theme,0,sizeof(*theme)); first = snprintf(theme->theme_id,sizeof(theme->theme_id),"%s",theme_id); second = snprintf(theme->name,sizeof(theme->name),"%s",name); if (first < 0 || second < 0 || (size_t)first >= sizeof(theme->theme_id) || (size_t)second >= sizeof(theme->name)) return UMI_STATUS_CAPACITY_EXCEEDED;
    theme->mode = mode; theme->density = density; theme->font_scale = 1.0; theme->revision = 1U; palette(theme); return UMI_STATUS_OK;
}
UmiStatus umi_ui_theme_profile_v2_validate(const UmiUiThemeProfileV2 *theme,char *out_reason,size_t capacity)
{
    int length;
    if (theme == NULL || out_reason == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (theme->font_scale < 0.75 || theme->font_scale > 2.0) { length = snprintf(out_reason,capacity,"Font scale must be between 0.75 and 2.0"); return length < 0 || (size_t)length >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_INVALID_STATE; }
    if (theme->accent[0] != '#' || theme->background[0] != '#' || theme->surface[0] != '#' || theme->foreground[0] != '#') { length = snprintf(out_reason,capacity,"Theme colour tokens must use hexadecimal notation"); return length < 0 || (size_t)length >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_INVALID_STATE; }
    length = snprintf(out_reason,capacity,"Theme is valid"); return length < 0 || (size_t)length >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}
