/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/preferences.c
 *
 * PURPOSE:
 *   Provide complete designer defaults and validation for grid, snap, autosave,
 *   panel visibility, session restoration and appearance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/preferences.h"
#include "internal.h"
#include <math.h>


UmiWorkbenchDesignerPreferences umi_workbench_designer_preferences_default(void)
{
    UmiWorkbenchDesignerPreferences preferences;
    (void)memset(&preferences, 0, sizeof(preferences));
    preferences.show_grid = true;
    preferences.snap_to_grid = true;
    preferences.show_alignment_guides = true;
    preferences.show_rulers = true;
    preferences.show_minimap = true;
    preferences.show_outline = true;
    preferences.show_properties = true;
    preferences.show_collaboration = true;
    preferences.autosave_enabled = true;
    preferences.confirm_destructive_commands = true;
    preferences.restore_last_session = true;
    preferences.grid_size = 8.0;
    preferences.snap_threshold = 6.0;
    preferences.default_zoom = 1.0;
    preferences.autosave_debounce_ms = 1200U;
    preferences.autosave_maximum_delay_ms = 10000U;
    preferences.theme_mode = UMI_WORKBENCH_DESIGNER_THEME_SYSTEM;
    (void)umi_workbench_designer_copy_text(
        preferences.default_layout_category,
        sizeof(preferences.default_layout_category), "development");
    preferences.revision = 1U;
    return preferences;
}

static UmiStatus preferences_error(
    char *error,
    size_t capacity,
    const char *message)
{
    if (error != NULL && capacity > 0U) {
        UmiStatus status = umi_workbench_designer_copy_text(error, capacity, message);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_INVALID_ARGUMENT;
}

UmiStatus umi_workbench_designer_preferences_validate(
    const UmiWorkbenchDesignerPreferences *preferences,
    char *error,
    size_t error_capacity)
{
    if (error != NULL && error_capacity > 0U) error[0] = '\0';
    if (preferences == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!isfinite(preferences->grid_size) || preferences->grid_size < 1.0 ||
        preferences->grid_size > 256.0) {
        return preferences_error(error, error_capacity,
            "Grid size must be between 1 and 256 logical units.");
    }
    if (!isfinite(preferences->snap_threshold) ||
        preferences->snap_threshold < 0.0 || preferences->snap_threshold > 64.0) {
        return preferences_error(error, error_capacity,
            "Snap threshold must be between 0 and 64 logical units.");
    }
    if (!isfinite(preferences->default_zoom) ||
        preferences->default_zoom < 0.1 || preferences->default_zoom > 8.0) {
        return preferences_error(error, error_capacity,
            "Default zoom must be between 0.1 and 8.0.");
    }
    if (preferences->autosave_enabled &&
        (preferences->autosave_debounce_ms == 0U ||
         preferences->autosave_maximum_delay_ms < preferences->autosave_debounce_ms)) {
        return preferences_error(error, error_capacity,
            "Autosave maximum delay must be at least the debounce interval.");
    }
    if (preferences->theme_mode < UMI_WORKBENCH_DESIGNER_THEME_SYSTEM ||
        preferences->theme_mode > UMI_WORKBENCH_DESIGNER_THEME_HIGH_CONTRAST) {
        return preferences_error(error, error_capacity,
            "Theme mode is outside the supported range.");
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_designer_preferences_overlay(
    UmiWorkbenchDesignerPreferences *destination,
    const UmiWorkbenchDesignerPreferences *source)
{
    char error[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
    UmiStatus status;
    if (destination == NULL || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_designer_preferences_validate(
        source, error, sizeof(error));
    if (status != UMI_STATUS_OK) return status;
    *destination = *source;
    destination->revision += 1U;
    return UMI_STATUS_OK;
}

bool umi_workbench_designer_preferences_equal(
    const UmiWorkbenchDesignerPreferences *left,
    const UmiWorkbenchDesignerPreferences *right)
{
    UmiWorkbenchDesignerPreferences left_copy;
    UmiWorkbenchDesignerPreferences right_copy;
    if (left == NULL || right == NULL) return false;
    left_copy = *left;
    right_copy = *right;
    left_copy.revision = 0U;
    right_copy.revision = 0U;
    return memcmp(&left_copy, &right_copy, sizeof(left_copy)) == 0;
}
