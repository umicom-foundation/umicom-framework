/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/editor_theme.c
 *
 * PURPOSE:
 *   Resolve semantic appearance tokens into safe editor theme snapshots.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/editor_theme.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

static void copy_text(char *destination, size_t capacity, const char *source)
{
    if (destination == NULL || capacity == 0U) return;
    (void)snprintf(destination, capacity, "%s", source != NULL ? source : "");
}

static int valid_colour(const char *colour)
{
    size_t index;
    if (colour == NULL || colour[0] != '#' || strlen(colour) != 7U) return 0;
    for (index = 1U; index < 7U; ++index) {
        if (!isxdigit((unsigned char)colour[index])) return 0;
    }
    return 1;
}

static void add_scheme(UmiUiEditorThemeSnapshot *theme, const char *scheme_id)
{
    size_t index = theme->preferred_scheme_count;
    if (index >= UMI_UI_EDITOR_THEME_SCHEME_CAPACITY) return;
    copy_text(theme->preferred_scheme_ids[index],
              sizeof(theme->preferred_scheme_ids[index]), scheme_id);
    ++theme->preferred_scheme_count;
}

UmiStatus umi_ui_editor_theme_resolve(
    const UmiUiAppearanceProfile *appearance,
    UmiUiEditorThemeSnapshot *out_theme)
{
    char reason[192U];
    if (appearance == NULL || out_theme == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_ui_appearance_profile_validate(appearance, reason,
                                           sizeof(reason)) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_theme, 0, sizeof(*out_theme));
    out_theme->struct_size = (uint32_t)sizeof(*out_theme);
    out_theme->api_version = UMI_UI_EDITOR_THEME_API_VERSION;
    out_theme->mode = appearance->mode;
    copy_text(out_theme->background, sizeof(out_theme->background),
              appearance->editor_background);
    copy_text(out_theme->foreground, sizeof(out_theme->foreground),
              appearance->foreground);
    copy_text(out_theme->current_line, sizeof(out_theme->current_line),
              appearance->mode == UMI_UI_THEME_MODE_HIGH_CONTRAST
                  ? appearance->accent_surface
                  : appearance->hover_surface);
    copy_text(out_theme->selection, sizeof(out_theme->selection),
              appearance->accent_surface);
    copy_text(out_theme->gutter, sizeof(out_theme->gutter),
              appearance->surface);
    copy_text(out_theme->gutter_foreground,
              sizeof(out_theme->gutter_foreground),
              appearance->muted_foreground);
    copy_text(out_theme->caret, sizeof(out_theme->caret), appearance->accent);
    copy_text(out_theme->right_margin, sizeof(out_theme->right_margin),
              appearance->border);
    copy_text(out_theme->font_family, sizeof(out_theme->font_family),
              appearance->editor_font);
    out_theme->font_size = appearance->editor_font_size *
                           appearance->font_scale;
    out_theme->highlight_current_line = 1;

    if (appearance->mode == UMI_UI_THEME_MODE_DARK ||
        appearance->mode == UMI_UI_THEME_MODE_HIGH_CONTRAST) {
        add_scheme(out_theme, "Adwaita-dark");
        add_scheme(out_theme, "oblivion");
        add_scheme(out_theme, "cobalt");
    } else {
        add_scheme(out_theme, "Adwaita");
        add_scheme(out_theme, "classic");
        add_scheme(out_theme, "tango");
    }
    return umi_ui_editor_theme_validate(out_theme, reason, sizeof(reason));
}

UmiStatus umi_ui_editor_theme_validate(
    const UmiUiEditorThemeSnapshot *theme,
    char *out_reason,
    size_t capacity)
{
    const char *reason = "Editor theme is valid";
    UmiStatus status = UMI_STATUS_OK;
    size_t index;
    int written;
    if (theme == NULL || out_reason == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (theme->struct_size != sizeof(*theme) ||
        theme->api_version != UMI_UI_EDITOR_THEME_API_VERSION) {
        reason = "Editor theme structure or API version is invalid";
        status = UMI_STATUS_INVALID_STATE;
    } else if (theme->mode < UMI_UI_THEME_MODE_SYSTEM ||
               theme->mode > UMI_UI_THEME_MODE_HIGH_CONTRAST) {
        reason = "Editor theme mode is invalid";
        status = UMI_STATUS_INVALID_STATE;
    } else if (!valid_colour(theme->background) ||
               !valid_colour(theme->foreground) ||
               !valid_colour(theme->current_line) ||
               !valid_colour(theme->selection) ||
               !valid_colour(theme->gutter) ||
               !valid_colour(theme->gutter_foreground) ||
               !valid_colour(theme->caret) ||
               !valid_colour(theme->right_margin)) {
        reason = "Every editor colour must use #RRGGBB notation";
        status = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(theme->background, theme->current_line) == 0) {
        reason = "Current-line colour must remain distinguishable";
        status = UMI_STATUS_INVALID_STATE;
    } else if ((theme->mode == UMI_UI_THEME_MODE_DARK ||
                theme->mode == UMI_UI_THEME_MODE_HIGH_CONTRAST) &&
               strcmp(theme->current_line, "#FFFFFF") == 0) {
        reason = "A dark editor cannot use an opaque white current line";
        status = UMI_STATUS_INVALID_STATE;
    } else if (theme->font_family[0] == '\0' || theme->font_size < 6.0 ||
               theme->font_size > 96.0 ||
               (theme->highlight_current_line != 0 &&
                theme->highlight_current_line != 1) ||
               theme->preferred_scheme_count == 0U ||
               theme->preferred_scheme_count >
                   UMI_UI_EDITOR_THEME_SCHEME_CAPACITY) {
        reason = "Editor typography, highlight or scheme list is invalid";
        status = UMI_STATUS_INVALID_STATE;
    } else {
        for (index = 0U; index < theme->preferred_scheme_count; ++index) {
            if (theme->preferred_scheme_ids[index][0] == '\0') {
                reason = "Preferred editor scheme identifiers cannot be empty";
                status = UMI_STATUS_INVALID_STATE;
                break;
            }
        }
    }
    written = snprintf(out_reason, capacity, "%s", reason);
    if (written < 0 || (size_t)written >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return status;
}

UmiStatus umi_ui_editor_theme_preferred_scheme(
    const UmiUiEditorThemeSnapshot *theme,
    size_t index,
    const char **out_scheme_id)
{
    if (theme == NULL || out_scheme_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_scheme_id = NULL;
    if (index >= theme->preferred_scheme_count) return UMI_STATUS_NOT_FOUND;
    *out_scheme_id = theme->preferred_scheme_ids[index];
    return UMI_STATUS_OK;
}
