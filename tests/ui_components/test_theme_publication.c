/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_components/test_theme_publication.c
 *
 * PURPOSE:
 *   Verify theme initialization captures borrowed labels and commits only a complete canonical palette.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/



#include "umicom/ui/theme_profile.h"
#include "umicom/ui/brand_palette.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expression) do { if (!(expression)) { \
    (void)fprintf(stderr, "line %d: %s\n", __LINE__, #expression); return 1; \
} } while (0)

int main(void)
{
    UmiUiThemeProfile value = {0}, before;
    char long_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char long_name[UMI_UI_WORKSPACE_LAYOUT_NAME_CAPACITY];
    (void)memset(long_id, 'i', sizeof(long_id));
    (void)memset(long_name, 'n', sizeof(long_name));
    CHECK(umi_ui_theme_profile_init(&value, "dark", "Original",
        UMI_UI_THEME_MODE_DARK, UMI_UI_DENSITY_COMFORTABLE) == UMI_STATUS_OK);
    (void)memcpy(&before, &value, sizeof(value));
    CHECK(umi_ui_theme_profile_init(&value, "replacement", long_name,
        UMI_UI_THEME_MODE_LIGHT, UMI_UI_DENSITY_COMPACT) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&value, &before, sizeof(value)) == 0);
    CHECK(umi_ui_theme_profile_init(&value, long_id, "replacement",
        UMI_UI_THEME_MODE_LIGHT, UMI_UI_DENSITY_COMPACT) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&value, &before, sizeof(value)) == 0);
    CHECK(umi_ui_theme_profile_init(&value, "", "replacement",
        UMI_UI_THEME_MODE_LIGHT, UMI_UI_DENSITY_COMPACT) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&value, &before, sizeof(value)) == 0);
    CHECK(umi_ui_theme_profile_init(&value, "replacement", "replacement",
        (UmiUiThemeMode)99, UMI_UI_DENSITY_COMPACT) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&value, &before, sizeof(value)) == 0);
    CHECK(umi_ui_theme_profile_init(&value, "replacement", "replacement",
        UMI_UI_THEME_MODE_LIGHT, (UmiUiDensity)0) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&value, &before, sizeof(value)) == 0);
    CHECK(umi_ui_theme_profile_init(&value, value.name, value.theme_id,
        UMI_UI_THEME_MODE_LIGHT, UMI_UI_DENSITY_SPACIOUS) == UMI_STATUS_OK);
    CHECK(strcmp(value.theme_id, "Original") == 0);
    CHECK(strcmp(value.name, "dark") == 0);
    CHECK(value.revision == 1U && value.font_scale == 1.0);

    const UmiUiThemeMode modes[] = {UMI_UI_THEME_MODE_SYSTEM, UMI_UI_THEME_MODE_LIGHT,
        UMI_UI_THEME_MODE_DARK, UMI_UI_THEME_MODE_HIGH_CONTRAST};
    const UmiUiBrandSurface surfaces[] = {UMI_UI_BRAND_SURFACE_LIGHT, UMI_UI_BRAND_SURFACE_LIGHT,
        UMI_UI_BRAND_SURFACE_DARK, UMI_UI_BRAND_SURFACE_HIGH_CONTRAST};
    for (size_t index = 0U; index < sizeof(modes) / sizeof(modes[0]); ++index) {
        char reason[256];
        CHECK(umi_ui_theme_profile_init(&value, "theme", "Theme", modes[index],
            UMI_UI_DENSITY_COMFORTABLE) == UMI_STATUS_OK);
        CHECK(umi_ui_theme_profile_validate(&value, reason, sizeof(reason)) == UMI_STATUS_OK);
        const UmiUiBrandPalette *palette = umi_ui_brand_palette_get(surfaces[index]);
        CHECK(palette != NULL);
        CHECK(strcmp(value.accent, palette->accent) == 0);
        CHECK(strcmp(value.background, palette->background) == 0);
        CHECK(strcmp(value.surface, palette->surface) == 0);
        CHECK(strcmp(value.foreground, palette->foreground) == 0);
        CHECK(strcmp(value.success, palette->success) == 0);
        CHECK(strcmp(value.warning, palette->warning) == 0);
        CHECK(strcmp(value.danger, palette->danger) == 0);
    }
    long_id[sizeof(long_id) - 1U] = '\0';
    long_name[sizeof(long_name) - 1U] = '\0';
    CHECK(umi_ui_theme_profile_init(&value, long_id, long_name,
        UMI_UI_THEME_MODE_DARK, UMI_UI_DENSITY_COMPACT) == UMI_STATUS_OK);
    CHECK(strlen(value.theme_id) == sizeof(long_id) - 1U);
    CHECK(strlen(value.name) == sizeof(long_name) - 1U);
    (void)memcpy(&before, &value, sizeof(value));
    CHECK(umi_ui_theme_profile_init(&value, NULL, "name",
        UMI_UI_THEME_MODE_DARK, UMI_UI_DENSITY_COMPACT) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&value, &before, sizeof(value)) == 0);
    CHECK(umi_ui_theme_profile_init(NULL, "id", "name",
        UMI_UI_THEME_MODE_DARK, UMI_UI_DENSITY_COMPACT) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
