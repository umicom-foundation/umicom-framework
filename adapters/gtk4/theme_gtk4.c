/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/theme_gtk4.c
 *
 * PURPOSE:
 *   Apply semantic Framework theme tokens through a private GTK4 CSS provider.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "gtk4_internal.h"

#include <string.h>

/*
 * Colours are semantic rather than tied to individual controls. The palette
 * can therefore evolve independently of the workbench hierarchy, and future
 * products can select another profile through the shared context store.
 */
static const char DARK_PALETTE[] =
    "@define-color umi_background #151b21;"
    "@define-color umi_surface #1d2731;"
    "@define-color umi_surface_raised #263543;"
    "@define-color umi_surface_hover #304253;"
    "@define-color umi_editor #171d23;"
    "@define-color umi_border #34495c;"
    "@define-color umi_border_strong #506579;"
    "@define-color umi_foreground #e8edf2;"
    "@define-color umi_muted #a0b4c5;"
    "@define-color umi_accent #c84c55;"
    "@define-color umi_accent_surface #4a252b;"
    "@define-color umi_success #54c98a;"
    "@define-color umi_warning #e7b85c;"
    "@define-color umi_danger #f07878;";

static const char LIGHT_PALETTE[] =
    "@define-color umi_background #eef1f4;"
    "@define-color umi_surface #ffffff;"
    "@define-color umi_surface_raised #f7f8fa;"
    "@define-color umi_surface_hover #e8edf4;"
    "@define-color umi_editor #ffffff;"
    "@define-color umi_border #d4d9e1;"
    "@define-color umi_border_strong #b8c0cc;"
    "@define-color umi_foreground #243342;"
    "@define-color umi_muted #637180;"
    "@define-color umi_accent #72161a;"
    "@define-color umi_accent_surface #f5e1e3;"
    "@define-color umi_success #237a45;"
    "@define-color umi_warning #96650c;"
    "@define-color umi_danger #b4232d;";

static const char HIGH_CONTRAST_PALETTE[] =
    "@define-color umi_background #000000;"
    "@define-color umi_surface #000000;"
    "@define-color umi_surface_raised #111111;"
    "@define-color umi_surface_hover #222222;"
    "@define-color umi_editor #000000;"
    "@define-color umi_border #ffffff;"
    "@define-color umi_border_strong #ffffff;"
    "@define-color umi_foreground #ffffff;"
    "@define-color umi_muted #d7d7d7;"
    "@define-color umi_accent #ffff00;"
    "@define-color umi_accent_surface #303000;"
    "@define-color umi_success #00ff80;"
    "@define-color umi_warning #ffff00;"
    "@define-color umi_danger #ff5f5f;";

/*
 * Keep every independently translated string literal within ISO C's portable
 * 4,095-character minimum.  Adjacent literals are concatenated during
 * translation, so one monolithic stylesheet still counts as one literal even
 * when it is visually split across many source lines.
 */
static const char WORKBENCH_CSS_CHROME[] =
    "* { -gtk-icon-style: symbolic; }"
    "window.umicom-workbench {"
    "  background-color: @umi_background; color: @umi_foreground;"
    "}"
    ".umicom-workbench-root { background-color: @umi_background; }"
    ".umicom-menubar {"
    "  min-height: 28px; padding: 1px 6px;"
    "  background-color: @umi_surface;"
    "  border-bottom: 1px solid @umi_border;"
    "}"
    ".umicom-menu-button > button {"
    "  min-height: 24px; padding: 2px 8px; margin: 0;"
    "  border: 0; border-radius: 4px; background: transparent;"
    "  color: @umi_foreground; box-shadow: none;"
    "}"
    ".umicom-menu-button > button:hover { background: @umi_surface_hover; }"
    ".umicom-menu-popover > contents {"
    "  min-width: 330px; padding: 4px; border: 1px solid @umi_border_strong;"
    "  border-radius: 7px; background: @umi_surface_raised;"
    "  box-shadow: 0 8px 24px alpha(black, 0.28);"
    "}"
    ".umicom-menu-item {"
    "  min-height: 30px; padding: 3px 8px; border: 0; border-radius: 4px;"
    "  background: transparent; color: @umi_foreground; box-shadow: none;"
    "}"
    ".umicom-menu-item:hover { background: @umi_surface_hover; }"
    ".umicom-menu-item.checked { background: @umi_accent_surface; }"
    ".umicom-menu-item:disabled { color: @umi_muted; opacity: 0.62; }"
    ".umicom-menu-state { color: @umi_accent; }"
    ".umicom-menu-icon { color: @umi_muted; }"
    ".umicom-menu-label { color: @umi_foreground; }"
    ".umicom-menu-accelerator {"
    "  min-width: 84px; color: @umi_muted; font-size: 0.88em;"
    "}"
    ".umicom-menu-separator { margin: 4px 6px; background: @umi_border; }"
    ".umicom-main-toolbar {"
    "  min-height: 38px; padding: 4px 8px;"
    "  background-color: @umi_surface_raised;"
    "  border-bottom: 1px solid @umi_border;"
    "}"
    ".umicom-project-widget { padding: 0 8px 0 3px; }"
    ".umicom-project-title { font-weight: 700; }"
    ".umicom-toolbar-button {"
    "  min-height: 28px; padding: 3px 8px; border-radius: 5px;"
    "  color: @umi_foreground;"
    "}"
    ".umicom-toolbar-button:hover { background: @umi_surface_hover; }"
    ".umicom-toolbar-separator { margin: 5px 3px; background: @umi_border; }";

static const char WORKBENCH_CSS_CONTROLS[] =
    ".umicom-workspace-profile-button > button {"
    "  min-height: 28px; padding: 3px 9px; margin-right: 4px;"
    "  border: 1px solid @umi_border_strong; border-radius: 5px;"
    "  background: @umi_surface; color: @umi_foreground; box-shadow: none;"
    "}"
    ".umicom-workspace-profile-button > button:hover {"
    "  background: @umi_surface_hover; border-color: @umi_accent;"
    "}"
    ".umicom-appearance-button > button {"
    "  min-height: 28px; padding: 3px 9px; margin-right: 2px;"
    "  border: 1px solid @umi_border_strong; border-radius: 5px;"
    "  background: @umi_surface; color: @umi_foreground; box-shadow: none;"
    "}"
    ".umicom-appearance-button > button:hover {"
    "  background: @umi_surface_hover; border-color: @umi_accent;"
    "}"
    ".umicom-workspace-profile-popover > contents {"
    "  min-width: 390px; padding: 7px;"
    "  border: 1px solid @umi_border_strong; border-radius: 7px;"
    "  background: @umi_surface_raised;"
    "}"
    ".umicom-workspace-profile-item {"
    "  min-height: 38px; padding: 5px 8px; border-radius: 5px;"
    "  color: @umi_foreground;"
    "}"
    ".umicom-workspace-profile-item:hover { background: @umi_surface_hover; }"
    ".umicom-workspace-profile-item.active { background: @umi_accent_surface; }"
    ".umicom-workspace-profile-description {"
    "  color: @umi_muted; font-size: 0.88em;"
    "}"
    ".umicom-appearance-popover > contents {"
    "  min-width: 440px; padding: 8px;"
    "  border: 1px solid @umi_border_strong; border-radius: 8px;"
    "  background: @umi_surface_raised;"
    "  box-shadow: 0 10px 28px alpha(black, 0.32);"
    "}"
    ".umicom-appearance-header { padding: 6px 8px 9px 8px; }"
    ".umicom-brand-logo {"
    "  margin-bottom: 7px; padding: 6px; border-radius: 6px;"
    "  background: #ffffff;"
    "}"
    ".umicom-brand-icon { margin: 0 1px; }"
    ".umicom-appearance-title {"
    "  color: @umi_foreground; font-size: 1.08em; font-weight: 700;"
    "}"
    ".umicom-appearance-description {"
    "  color: @umi_muted; font-size: 0.88em;"
    "}"
    "button.umicom-appearance-item {"
    "  min-height: 48px; padding: 7px 9px; border: 0;"
    "  border-left: 3px solid transparent; border-radius: 5px;"
    "  background: transparent; color: @umi_foreground; box-shadow: none;"
    "}"
    "button.umicom-appearance-item:hover { background: @umi_surface_hover; }"
    "button.umicom-appearance-item.active {"
    "  border-left-color: @umi_accent; background: @umi_accent_surface;"
    "}"
    ".umicom-appearance-item-title { font-weight: 700; }"
    ".umicom-layout-designer {"
    "  margin-top: 4px; padding: 7px; border-radius: 6px;"
    "  background: @umi_surface; border: 1px solid @umi_border;"
    "}"
    ".umicom-layout-designer-title {"
    "  color: @umi_foreground; font-weight: 700; font-size: 1.02em;"
    "}"
    ".umicom-layout-designer entry, .umicom-layout-designer spinbutton {"
    "  min-height: 28px; border-radius: 4px;"
    "  background: @umi_editor; color: @umi_foreground;"
    "}"
    ".umicom-layout-designer button {"
    "  min-height: 27px; padding: 3px 7px; border-radius: 4px;"
    "}";

static const char WORKBENCH_CSS_NAVIGATION[] =
    ".umicom-command-search {"
    "  min-height: 28px; padding: 2px 9px;"
    "  border: 1px solid @umi_border_strong; border-radius: 6px;"
    "  background: @umi_background; color: @umi_foreground;"
    "}"
    ".umicom-command-search:focus {"
    "  border-color: @umi_accent; box-shadow: 0 0 0 1px @umi_accent;"
    "}"
    ".umicom-command-results {"
    "  margin: 4px 160px 8px 160px; padding: 5px;"
    "  border: 1px solid @umi_border_strong; border-radius: 6px;"
    "  background: @umi_surface_raised; color: @umi_foreground;"
    "  box-shadow: 0 10px 30px alpha(black, 0.32);"
    "}"
    ".umicom-command-result {"
    "  min-height: 58px; padding: 0; border-radius: 5px;"
    "  border-left: 3px solid transparent;"
    "}"
    ".umicom-command-result:hover { background: @umi_surface_hover; }"
    ".umicom-command-result:selected {"
    "  background: @umi_accent_surface; border-left-color: @umi_accent;"
    "}"
    ".umicom-command-result-content { padding: 6px 9px; }"
    ".umicom-command-icon { color: @umi_accent; }"
    ".umicom-command-title { color: @umi_foreground; font-weight: 700; }"
    ".umicom-command-description { color: @umi_muted; font-size: 0.9em; }"
    ".umicom-command-meta { color: @umi_muted; font-size: 0.82em; }"
    ".umicom-command-accelerator {"
    "  min-width: 100px; padding: 3px 7px; border-radius: 4px;"
    "  background: @umi_surface; color: @umi_muted; font-size: 0.86em;"
    "}"
    ".umicom-left-cluster, .umicom-primary-sidebar,"
    ".umicom-auxiliary-sidebar, .umicom-bottom-panel {"
    "  background: @umi_surface;"
    "}"
    ".umicom-activity-rail {"
    "  padding: 5px 4px; background: @umi_background;"
    "  border-right: 1px solid @umi_border;"
    "}"
    "button.umicom-activity-button {"
    "  min-width: 38px; min-height: 38px; padding: 7px; margin: 1px;"
    "  border: 0; border-left: 3px solid transparent;"
    "  border-radius: 4px; background: transparent;"
    "  color: @umi_muted; box-shadow: none;"
    "}"
    "button.umicom-activity-button:hover {"
    "  background: @umi_surface_hover; color: @umi_foreground;"
    "}"
    "button.umicom-activity-button.active {"
    "  background: @umi_accent_surface; color: @umi_accent;"
    "  border-left-color: @umi_accent;"
    "}"
    ".umicom-tool-header {"
    "  min-height: 31px; padding: 6px 10px;"
    "  font-size: 0.92em; font-weight: 700;"
    "  background: @umi_surface;"
    "  border-bottom: 1px solid @umi_border;"
    "}"
    "paned.umicom-primary-split > separator {"
    "  min-width: 3px; min-height: 3px;"
    "  background: @umi_border;"
    "}"
    "paned.umicom-primary-split > separator:hover { background: @umi_accent; }"
    "notebook.umicom-tool-notebook > header {"
    "  background: @umi_surface_raised; border-bottom: 1px solid @umi_border;"
    "}"
    "notebook.umicom-tool-notebook > header tabs > tab {"
    "  min-height: 25px; padding: 4px 9px;"
    "  border: 0; border-bottom: 2px solid transparent;"
    "  color: @umi_muted; background: transparent;"
    "}"
    "notebook.umicom-tool-notebook > header tabs > tab:hover {"
    "  color: @umi_foreground; background: @umi_surface_hover;"
    "}"
    "notebook.umicom-tool-notebook > header tabs > tab:checked {"
    "  color: @umi_foreground; border-bottom-color: @umi_accent;"
    "  background: @umi_surface;"
    "}";

static const char WORKBENCH_CSS_EDITOR[] =
    ".umicom-tool-page { padding: 0; background: @umi_surface; }"
    ".umicom-editor-area { background: @umi_editor; }"
    "paned.umicom-editor-split > separator {"
    "  min-width: 4px; min-height: 4px; background: @umi_border_strong;"
    "}"
    "paned.umicom-editor-split > separator:hover { background: @umi_accent; }"
    "notebook.umicom-document-notebook {"
    "  border: 1px solid transparent; background: @umi_editor;"
    "}"
    "notebook.umicom-document-notebook.active {"
    "  border-color: @umi_border_strong;"
    "}"
    ".umicom-breadcrumbs {"
    "  min-height: 27px; padding: 3px 10px;"
    "  background: @umi_editor; color: @umi_muted;"
    "  border-bottom: 1px solid @umi_border;"
    "}"
    "notebook.umicom-document-notebook > header {"
    "  background: @umi_surface; border-bottom: 1px solid @umi_border;"
    "}"
    "notebook.umicom-document-notebook > header tabs > tab {"
    "  min-height: 29px; padding: 5px 12px;"
    "  border-right: 1px solid @umi_border;"
    "  border-top: 2px solid transparent;"
    "  color: @umi_muted; background: @umi_surface;"
    "}"
    "notebook.umicom-document-notebook > header tabs > tab:hover {"
    "  color: @umi_foreground; background: @umi_surface_hover;"
    "}"
    "notebook.umicom-document-notebook > header tabs > tab:checked {"
    "  color: @umi_foreground; background: @umi_editor;"
    "  border-top-color: @umi_accent;"
    "}"
    ".umicom-document-tab { padding: 0 2px; }"
    ".umicom-document-tab.preview label { font-style: italic; }"
    ".umicom-document-icon, .umicom-document-state {"
    "  color: @umi_muted; -gtk-icon-size: 14px;"
    "}"
    "button.umicom-document-tab-button {"
    "  min-width: 20px; min-height: 20px; padding: 2px; margin: 0 0 0 2px;"
    "  border: 0; border-radius: 3px; background: transparent;"
    "  color: @umi_muted; box-shadow: none;"
    "}"
    "button.umicom-document-tab-button:hover {"
    "  background: @umi_surface_hover; color: @umi_foreground;"
    "}"
    "textview.umicom-editor, textview.umicom-editor text {"
    "  background: @umi_editor; color: @umi_foreground;"
    "  caret-color: @umi_accent; font-family: monospace;"
    "}"
    "textview.umicom-editor selection, textview.umicom-editor text selection {"
    "  background: @umi_accent_surface; color: @umi_foreground;"
    "}"
    ".umicom-editor-scroll { background: @umi_editor; }"
    ".umicom-notification-banner {"
    "  margin: 5px 10px; padding: 7px 10px;"
    "  border-left: 3px solid @umi_accent; border-radius: 4px;"
    "  background: @umi_accent_surface; color: @umi_foreground;"
    "}"
    ".umicom-statusbar {"
    "  min-height: 22px; padding: 2px 8px;"
    "  background: @umi_accent_surface; color: @umi_foreground;"
    "  border-top: 1px solid @umi_border; font-size: 0.88em;"
    "}"
    ".umicom-status-context { color: @umi_muted; }"
    ".accent { color: @umi_accent; }"
    "scrollbar slider { min-width: 8px; min-height: 8px; border-radius: 8px;"
    "  background: @umi_border_strong; }"
    "scrollbar slider:hover { background: @umi_muted; }";

static const char WORKBENCH_CSS_DESKTOP[] =
    ".umicom-desktop-layout-bar {"
    "  min-height: 31px; padding: 2px 6px; background: @umi_surface_raised;"
    "  border-top: 1px solid @umi_border;"
    "}"
    "button.umicom-desktop-layout-tab {"
    "  min-height: 25px; padding: 2px 9px; border: 0; border-radius: 4px;"
    "  color: @umi_muted; background: transparent; box-shadow: none;"
    "}"
    "button.umicom-desktop-layout-tab:hover {"
    "  color: @umi_foreground; background: @umi_surface_hover;"
    "}"
    "button.umicom-desktop-layout-tab.active {"
    "  color: @umi_foreground; background: @umi_accent_surface;"
    "  border-bottom: 2px solid @umi_accent;"
    "}"
    ".umicom-desktop-layout-state { color: @umi_warning; font-weight: 700; }"
    ".umicom-desktop-monitor-label { color: @umi_muted; font-size: 0.86em; }"
    ".umicom-desktop-designer-button > button {"
    "  min-width: 30px; min-height: 25px; border-radius: 4px;"
    "  color: @umi_foreground;"
    "}"
    ".umicom-desktop-designer { min-width: 780px; padding: 10px; }"
    ".umicom-desktop-designer-title {"
    "  color: @umi_foreground; font-size: 1.08em; font-weight: 700;"
    "}"
    ".umicom-desktop-designer-summary { color: @umi_muted; }"
    ".umicom-desktop-canvas {"
    "  border: 1px solid @umi_border_strong; border-radius: 6px;"
    "  background: @umi_background;"
    "}"
    ".umicom-desktop-action { min-height: 28px; padding: 3px 8px; }";

_Static_assert(sizeof(WORKBENCH_CSS_CHROME) <= 4096U,
               "GTK4 chrome CSS exceeds ISO C's portable string limit");
_Static_assert(sizeof(WORKBENCH_CSS_CONTROLS) <= 4096U,
               "GTK4 controls CSS exceeds ISO C's portable string limit");
_Static_assert(sizeof(WORKBENCH_CSS_NAVIGATION) <= 4096U,
               "GTK4 navigation CSS exceeds ISO C's portable string limit");
_Static_assert(sizeof(WORKBENCH_CSS_EDITOR) <= 4096U,
               "GTK4 editor CSS exceeds ISO C's portable string limit");
_Static_assert(sizeof(WORKBENCH_CSS_DESKTOP) <= 4096U,
               "GTK4 desktop CSS exceeds ISO C's portable string limit");

static const char *palette_for_workbench(UmiUiWorkbench *workbench)
{
    UmiUiContextSnapshot context;
    if (workbench != NULL &&
        umi_ui_context_get(umi_ui_workbench_context(workbench),
                           "studio.ui.theme",
                           &context) == UMI_STATUS_OK &&
        context.kind == UMI_UI_CONTEXT_STRING) {
        if (strcmp(context.string_value, "light") == 0 ||
            strcmp(context.string_value, "umicom-light") == 0 ||
            strcmp(context.string_value, "umicom-system") == 0) {
            return LIGHT_PALETTE;
        }
        if (strcmp(context.string_value, "high-contrast") == 0 ||
            strcmp(context.string_value, "umicom-high-contrast") == 0) {
            return HIGH_CONTRAST_PALETTE;
        }
    }
    return DARK_PALETTE;
}

static char *appearance_css(UmiUiWorkbench *workbench,
                            int *out_prefer_dark)
{
    UmiUiAppearanceProfile profile;
    double interface_size;
    double editor_size;
    int control_height;
    int toolbar_padding;

    if (out_prefer_dark != NULL) *out_prefer_dark = 1;
    if (workbench == NULL ||
        umi_ui_appearance_model_active(
            umi_ui_workbench_appearance(workbench), &profile) !=
            UMI_STATUS_OK) {
        return NULL;
    }
    if (out_prefer_dark != NULL) {
        *out_prefer_dark = profile.mode == UMI_UI_THEME_MODE_DARK ||
                           profile.mode == UMI_UI_THEME_MODE_HIGH_CONTRAST;
    }
    interface_size = profile.interface_font_size * profile.font_scale;
    editor_size = profile.editor_font_size * profile.font_scale;
    control_height = profile.density == UMI_UI_DENSITY_COMPACT ? 25 :
        (profile.density == UMI_UI_DENSITY_SPACIOUS ? 34 : 29);
    toolbar_padding = profile.density == UMI_UI_DENSITY_COMPACT ? 3 :
        (profile.density == UMI_UI_DENSITY_SPACIOUS ? 7 : 5);

    return g_strdup_printf(
        "@define-color umi_background %s;"
        "@define-color umi_surface %s;"
        "@define-color umi_surface_raised %s;"
        "@define-color umi_surface_hover %s;"
        "@define-color umi_editor %s;"
        "@define-color umi_border %s;"
        "@define-color umi_border_strong %s;"
        "@define-color umi_foreground %s;"
        "@define-color umi_muted %s;"
        "@define-color umi_accent %s;"
        "@define-color umi_accent_surface %s;"
        "@define-color umi_success %s;"
        "@define-color umi_warning %s;"
        "@define-color umi_danger %s;"
        "window.umicom-workbench { font-family: %s; font-size: %.2fpt; }"
        "textview.umicom-editor, textview.umicom-editor text {"
        " font-family: %s; font-size: %.2fpt; }"
        ".umicom-main-toolbar { padding: %dpx 8px; }"
        ".umicom-toolbar-button, .umicom-workspace-profile-button > button,"
        ".umicom-appearance-button > button { min-height: %dpx; }",
        profile.background, profile.surface, profile.raised_surface,
        profile.hover_surface, profile.editor_background, profile.border,
        profile.strong_border, profile.foreground, profile.muted_foreground,
        profile.accent, profile.accent_surface, profile.success,
        profile.warning, profile.danger, profile.interface_font,
        interface_size, profile.editor_font, editor_size, toolbar_padding,
        control_height);
}

UmiStatus umi_gtk4_apply_theme(UmiGtk4Adapter *adapter,
                               UmiUiWorkbench *workbench)
{
    GdkDisplay *display;
    char *css;
    char *profile_css;
    const char *palette;
    GtkSettings *settings;
    int prefer_dark = 1;

    if (adapter == NULL || adapter->window == NULL || workbench == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    display = gtk_widget_get_display(GTK_WIDGET(adapter->window));
    if (adapter->theme_provider == NULL) {
        adapter->theme_provider = gtk_css_provider_new();
        adapter->theme_display = display;
        gtk_style_context_add_provider_for_display(
            display,
            GTK_STYLE_PROVIDER(adapter->theme_provider),
            GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    }

    profile_css = appearance_css(workbench, &prefer_dark);
    palette = profile_css != NULL ? profile_css
                                  : palette_for_workbench(workbench);
    /* Repeat the profile fragment after the structural stylesheet so its
     * configurable typography and density override conservative fallbacks. */
    css = g_strconcat(palette,
                      WORKBENCH_CSS_CHROME,
                      WORKBENCH_CSS_CONTROLS,
                      WORKBENCH_CSS_NAVIGATION,
                      WORKBENCH_CSS_EDITOR,
                      WORKBENCH_CSS_DESKTOP,
                      profile_css != NULL ? profile_css : "", NULL);
    g_free(profile_css);
    if (css == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    gtk_css_provider_load_from_string(adapter->theme_provider, css);
    g_free(css);
    settings = gtk_settings_get_for_display(display);
    if (settings != NULL) {
        g_object_set(settings, "gtk-application-prefer-dark-theme",
                     prefer_dark != 0, NULL);
    }
    return umi_gtk4_apply_editor_themes(adapter, workbench);
}
