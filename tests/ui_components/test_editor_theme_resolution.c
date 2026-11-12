/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_components/test_editor_theme_resolution.c
 *
 * PURPOSE:
 *   Verify editor resolution publishes only complete valid output and captures overlapping appearance input.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define CHECK(expression) do { \
    if (!(expression)) { \
        fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression); \
        return 1; \
    } \
} while (0)

#include "umicom/ui/editor_theme.h"

int main(void)
{
    UmiUiAppearanceProfile appearance = {0}, saved_appearance;
    UmiUiEditorThemeSnapshot output, before;
    char reason[256];
    CHECK(umi_ui_appearance_profile_init(&appearance, "dark", "Umicom Dark",
        UMI_UI_THEME_MODE_DARK, UMI_UI_DENSITY_COMFORTABLE) == UMI_STATUS_OK);
    CHECK(umi_ui_editor_theme_resolve(&appearance, &output) == UMI_STATUS_OK);
    memcpy(&before, &output, sizeof(before));
    /* A structurally valid appearance can still derive an unusable editor
     * highlight. The failed derivation must retain the previous snapshot. */
    (void)snprintf(appearance.hover_surface, sizeof(appearance.hover_surface), "%s",
                   appearance.editor_background);
    CHECK(umi_ui_appearance_profile_validate(&appearance, reason, sizeof(reason)) == UMI_STATUS_OK);
    CHECK(umi_ui_editor_theme_resolve(&appearance, &output) == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(&before, &output, sizeof(before)) == 0);
    appearance.font_scale = NAN;
    CHECK(umi_ui_editor_theme_resolve(&appearance, &output) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&before, &output, sizeof(before)) == 0);

    CHECK(umi_ui_appearance_profile_init(&appearance, "light", "Umicom Light",
        UMI_UI_THEME_MODE_LIGHT, UMI_UI_DENSITY_COMFORTABLE) == UMI_STATUS_OK);
    memcpy(&saved_appearance, &appearance, sizeof(saved_appearance));
    CHECK(umi_ui_appearance_profile_validate(&appearance, appearance.label, sizeof(appearance.label))
          == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&saved_appearance, &appearance, sizeof(saved_appearance)) == 0);
    /* Both complete object spans live in one sufficiently sized union. Staging
     * must read all input fields before publishing the overlapping result. */
    union {
        UmiUiAppearanceProfile appearance;
        UmiUiEditorThemeSnapshot editor;
    } shared;
    shared.appearance = appearance;
    CHECK(umi_ui_editor_theme_resolve(&shared.appearance, &shared.editor) == UMI_STATUS_OK);
    CHECK(shared.editor.mode == UMI_UI_THEME_MODE_LIGHT);
    CHECK(strcmp(shared.editor.font_family, appearance.editor_font) == 0);
    CHECK(strcmp(shared.editor.background, appearance.editor_background) == 0);
    CHECK(shared.editor.font_size == appearance.editor_font_size * appearance.font_scale);
    CHECK(umi_ui_editor_theme_validate(&shared.editor, reason, sizeof(reason)) == UMI_STATUS_OK);
    CHECK(umi_ui_editor_theme_resolve(NULL, &output) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_ui_editor_theme_resolve(&appearance, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
