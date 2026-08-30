/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/preferences.h
 *
 * PURPOSE:
 *   Define validated user preferences for the Layout Browser and visual designer
 *   without binding settings to GTK widgets or a private product store.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_PREFERENCES_H
#define UMICOM_WORKBENCH_DESIGNER_PREFERENCES_H

#include "umicom/workbench_designer/types.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum UmiWorkbenchDesignerThemeMode {
    UMI_WORKBENCH_DESIGNER_THEME_SYSTEM = 1,
    UMI_WORKBENCH_DESIGNER_THEME_LIGHT = 2,
    UMI_WORKBENCH_DESIGNER_THEME_DARK = 3,
    UMI_WORKBENCH_DESIGNER_THEME_HIGH_CONTRAST = 4
} UmiWorkbenchDesignerThemeMode;

typedef struct UmiWorkbenchDesignerPreferences {
    bool show_grid;
    bool snap_to_grid;
    bool show_alignment_guides;
    bool show_rulers;
    bool show_minimap;
    bool show_outline;
    bool show_properties;
    bool show_collaboration;
    bool autosave_enabled;
    bool confirm_destructive_commands;
    bool restore_last_session;
    double grid_size;
    double snap_threshold;
    double default_zoom;
    uint64_t autosave_debounce_ms;
    uint64_t autosave_maximum_delay_ms;
    UmiWorkbenchDesignerThemeMode theme_mode;
    char default_layout_category[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char default_export_directory[UMI_WORKBENCH_DESIGNER_PATH_CAPACITY];
    uint64_t revision;
} UmiWorkbenchDesignerPreferences;

UmiWorkbenchDesignerPreferences umi_workbench_designer_preferences_default(void);
UmiStatus umi_workbench_designer_preferences_validate(const UmiWorkbenchDesignerPreferences *preferences, char *error, size_t error_capacity);
UmiStatus umi_workbench_designer_preferences_overlay(UmiWorkbenchDesignerPreferences *destination, const UmiWorkbenchDesignerPreferences *source);
bool umi_workbench_designer_preferences_equal(const UmiWorkbenchDesignerPreferences *left, const UmiWorkbenchDesignerPreferences *right);

#ifdef __cplusplus
}
#endif

#endif
