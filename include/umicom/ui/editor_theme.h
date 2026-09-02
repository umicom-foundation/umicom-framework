/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/editor_theme.h
 *
 * PURPOSE:
 *   Resolve an appearance profile into a safe, toolkit-neutral editor theme.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Editor controls often have a second theming system for syntax and current-
 * line colours. This contract keeps those colours aligned with the Framework
 * appearance profile instead of accepting a toolkit's unrelated defaults.
 */

#ifndef UMICOM_UI_EDITOR_THEME_H
#define UMICOM_UI_EDITOR_THEME_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/ui/appearance.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_EDITOR_THEME_API_VERSION 1U
#define UMI_UI_EDITOR_THEME_SCHEME_CAPACITY 4U
#define UMI_UI_EDITOR_THEME_SCHEME_ID_CAPACITY 48U

/**
 * Represent the ui editor theme snapshot data shared with callers of this public contract.
 */
typedef struct UmiUiEditorThemeSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiUiThemeMode mode;
    char background[UMI_UI_APPEARANCE_COLOUR_CAPACITY];
    char foreground[UMI_UI_APPEARANCE_COLOUR_CAPACITY];
    char current_line[UMI_UI_APPEARANCE_COLOUR_CAPACITY];
    char selection[UMI_UI_APPEARANCE_COLOUR_CAPACITY];
    char gutter[UMI_UI_APPEARANCE_COLOUR_CAPACITY];
    char gutter_foreground[UMI_UI_APPEARANCE_COLOUR_CAPACITY];
    char caret[UMI_UI_APPEARANCE_COLOUR_CAPACITY];
    char right_margin[UMI_UI_APPEARANCE_COLOUR_CAPACITY];
    char font_family[UMI_UI_APPEARANCE_FONT_CAPACITY];
    double font_size;
    int highlight_current_line;
    size_t preferred_scheme_count;
    char preferred_scheme_ids[UMI_UI_EDITOR_THEME_SCHEME_CAPACITY]
                             [UMI_UI_EDITOR_THEME_SCHEME_ID_CAPACITY];
} UmiUiEditorThemeSnapshot;

/**
 * Provide the ui editor theme resolve operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_editor_theme_resolve(
    const UmiUiAppearanceProfile *appearance,
    UmiUiEditorThemeSnapshot *out_theme);
/**
 * Check that ui editor theme satisfies its contract before another service relies on it.
 */
UmiStatus umi_ui_editor_theme_validate(
    const UmiUiEditorThemeSnapshot *theme,
    char *out_reason,
    size_t capacity);
/**
 * Provide the ui editor theme preferred scheme operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_editor_theme_preferred_scheme(
    const UmiUiEditorThemeSnapshot *theme,
    size_t index,
    const char **out_scheme_id);

#ifdef __cplusplus
}
#endif

#endif
