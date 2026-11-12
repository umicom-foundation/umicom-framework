/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_components/test_editor_theme_admission.c
 *
 * PURPOSE:
 *   Verify bounded editor typography and scheme lookup, including nonfinite values and case-equivalent colours.
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
    UmiUiAppearanceProfile appearance = {0};
    UmiUiEditorThemeSnapshot valid, value, before;
    char reason[256];
    CHECK(umi_ui_appearance_profile_init(&appearance, "dark", "Umicom Dark",
        UMI_UI_THEME_MODE_DARK, UMI_UI_DENSITY_COMFORTABLE) == UMI_STATUS_OK);
    CHECK(umi_ui_editor_theme_resolve(&appearance, &valid) == UMI_STATUS_OK);
    const double bad_sizes[] = {NAN, INFINITY, -INFINITY, 5.99, 96.01};
    for (size_t i = 0U; i < sizeof(bad_sizes) / sizeof(bad_sizes[0]); ++i) {
        value = valid;
        value.font_size = bad_sizes[i];
        CHECK(umi_ui_editor_theme_validate(&value, reason, sizeof(reason)) == UMI_STATUS_INVALID_STATE);
    }
    value = valid;
    value.font_size = 6.0;
    CHECK(umi_ui_editor_theme_validate(&value, reason, sizeof(reason)) == UMI_STATUS_OK);
    value.font_size = 96.0;
    CHECK(umi_ui_editor_theme_validate(&value, reason, sizeof(reason)) == UMI_STATUS_OK);
    value = valid;
    memset(value.background, '#', sizeof(value.background));
    CHECK(umi_ui_editor_theme_validate(&value, reason, sizeof(reason)) == UMI_STATUS_INVALID_STATE);
    value = valid;
    memset(value.font_family, 'f', sizeof(value.font_family));
    CHECK(umi_ui_editor_theme_validate(&value, reason, sizeof(reason)) == UMI_STATUS_INVALID_STATE);
    (void)snprintf(value.font_family, sizeof(value.font_family), "%s", "Consolas,,Cascadia Code");
    CHECK(umi_ui_editor_theme_validate(&value, reason, sizeof(reason)) == UMI_STATUS_INVALID_STATE);
    (void)snprintf(value.font_family, sizeof(value.font_family), "%s", "Cascadia Code, Consolas");
    CHECK(umi_ui_editor_theme_validate(&value, reason, sizeof(reason)) == UMI_STATUS_OK);
    value = valid;
    (void)snprintf(value.background, sizeof(value.background), "%s", "#abcdef");
    (void)snprintf(value.current_line, sizeof(value.current_line), "%s", "#ABCDEF");
    CHECK(umi_ui_editor_theme_validate(&value, reason, sizeof(reason)) == UMI_STATUS_INVALID_STATE);
    value = valid;
    (void)snprintf(value.current_line, sizeof(value.current_line), "%s", "#ffffff");
    CHECK(umi_ui_editor_theme_validate(&value, reason, sizeof(reason)) == UMI_STATUS_INVALID_STATE);

    const char *borrowed = "unchanged";
    const size_t counts[] = {0U, UMI_UI_EDITOR_THEME_SCHEME_CAPACITY + 1U, SIZE_MAX};
    for (size_t i = 0U; i < sizeof(counts) / sizeof(counts[0]); ++i) {
        value = valid;
        value.preferred_scheme_count = counts[i];
        borrowed = "unchanged";
        CHECK(umi_ui_editor_theme_preferred_scheme(&value, UMI_UI_EDITOR_THEME_SCHEME_CAPACITY, &borrowed)
              == UMI_STATUS_INVALID_STATE);
        CHECK(borrowed == NULL);
    }
    value = valid;
    memset(value.preferred_scheme_ids[1], 's', sizeof(value.preferred_scheme_ids[1]));
    CHECK(umi_ui_editor_theme_preferred_scheme(&value, 0U, &borrowed) == UMI_STATUS_INVALID_STATE);
    CHECK(borrowed == NULL);
    value = valid;
    CHECK(umi_ui_editor_theme_preferred_scheme(&value, 0U, &borrowed) == UMI_STATUS_OK);
    CHECK(strcmp(borrowed, "Adwaita-dark") == 0);
    CHECK(umi_ui_editor_theme_preferred_scheme(&value, value.preferred_scheme_count, &borrowed)
          == UMI_STATUS_NOT_FOUND);
    CHECK(borrowed == NULL);
    memcpy(&before, &value, sizeof(before));
    CHECK(umi_ui_editor_theme_validate(&value, value.font_family, sizeof(value.font_family))
          == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_ui_editor_theme_preferred_scheme(&value, 0U, (const char **)(void *)&value)
          == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&before, &value, sizeof(before)) == 0);
    return 0;
}
