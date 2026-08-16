/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_ui_editor_theme.c
 *
 * PURPOSE:
 *   Verify safe editor themes for dark, light and high-contrast appearances.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/editor_theme.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

static void verify_mode(UmiUiThemeMode mode,
                        const char *expected_scheme,
                        int reject_white_line)
{
    UmiUiAppearanceProfile appearance;
    UmiUiEditorThemeSnapshot theme;
    const char *scheme_id = NULL;
    char reason[192U];

    assert(umi_ui_appearance_profile_init(
               &appearance, "test.appearance", "Test appearance", mode,
               UMI_UI_DENSITY_COMFORTABLE) == UMI_STATUS_OK);
    assert(umi_ui_editor_theme_resolve(&appearance, &theme) == UMI_STATUS_OK);
    assert(umi_ui_editor_theme_validate(
               &theme, reason, sizeof(reason)) == UMI_STATUS_OK);
    assert(strcmp(theme.background, theme.current_line) != 0);
    if (reject_white_line) {
        assert(strcmp(theme.current_line, "#FFFFFF") != 0);
    }
    assert(umi_ui_editor_theme_preferred_scheme(
               &theme, 0U, &scheme_id) == UMI_STATUS_OK);
    assert(strcmp(scheme_id, expected_scheme) == 0);
    assert(umi_ui_editor_theme_preferred_scheme(
               &theme, theme.preferred_scheme_count, &scheme_id) ==
           UMI_STATUS_NOT_FOUND);
}

int main(void)
{
    verify_mode(UMI_UI_THEME_MODE_DARK, "Adwaita-dark", 1);
    verify_mode(UMI_UI_THEME_MODE_LIGHT, "Adwaita", 0);
    verify_mode(UMI_UI_THEME_MODE_HIGH_CONTRAST, "Adwaita-dark", 1);
    return EXIT_SUCCESS;
}
