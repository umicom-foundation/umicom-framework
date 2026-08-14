/* Umicom Framework | Theme profile v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/ui/theme_profile.h"
#include <stdio.h>
#include <string.h>

static void palette(UmiUiThemeProfile *theme)
{
    const bool light = theme->mode == UMI_UI_THEME_MODE_LIGHT;
    const bool contrast = theme->mode == UMI_UI_THEME_MODE_HIGH_CONTRAST;
    (void)snprintf(theme->accent,sizeof(theme->accent),"%s",contrast ? "#ffff00" : "#4f8cff");
    (void)snprintf(theme->background,sizeof(theme->background),"%s",light ? "#f4f6fa" : "#0b1020");
    (void)snprintf(theme->surface,sizeof(theme->surface),"%s",light ? "#ffffff" : "#151d2f");
    (void)snprintf(theme->foreground,sizeof(theme->foreground),"%s",light ? "#111827" : "#eef3ff");
    (void)snprintf(theme->success,sizeof(theme->success),"#32c777");
    (void)snprintf(theme->warning,sizeof(theme->warning),"#f5b942");
    (void)snprintf(theme->danger,sizeof(theme->danger),"#ef5b5b");
}
UmiStatus umi_ui_theme_profile_init(UmiUiThemeProfile *theme,const char *theme_id,const char *name,UmiUiThemeMode mode,UmiUiDensity density)
{
    int first; int second;
    if (theme == NULL || theme_id == NULL || name == NULL || theme_id[0] == '\0' || name[0] == '\0' || mode < UMI_UI_THEME_MODE_SYSTEM || mode > UMI_UI_THEME_MODE_HIGH_CONTRAST || density < UMI_UI_DENSITY_COMPACT || density > UMI_UI_DENSITY_SPACIOUS) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(theme,0,sizeof(*theme)); first = snprintf(theme->theme_id,sizeof(theme->theme_id),"%s",theme_id); second = snprintf(theme->name,sizeof(theme->name),"%s",name); if (first < 0 || second < 0 || (size_t)first >= sizeof(theme->theme_id) || (size_t)second >= sizeof(theme->name)) return UMI_STATUS_CAPACITY_EXCEEDED;
    theme->mode = mode; theme->density = density; theme->font_scale = 1.0; theme->revision = 1U; palette(theme); return UMI_STATUS_OK;
}
UmiStatus umi_ui_theme_profile_validate(const UmiUiThemeProfile *theme,char *out_reason,size_t capacity)
{
    int length;
    if (theme == NULL || out_reason == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (theme->font_scale < 0.75 || theme->font_scale > 2.0) { length = snprintf(out_reason,capacity,"Font scale must be between 0.75 and 2.0"); return length < 0 || (size_t)length >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_INVALID_STATE; }
    if (theme->accent[0] != '#' || theme->background[0] != '#' || theme->surface[0] != '#' || theme->foreground[0] != '#') { length = snprintf(out_reason,capacity,"Theme colour tokens must use hexadecimal notation"); return length < 0 || (size_t)length >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_INVALID_STATE; }
    length = snprintf(out_reason,capacity,"Theme is valid"); return length < 0 || (size_t)length >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}
