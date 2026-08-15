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
    "@define-color umi_background #17191d;"
    "@define-color umi_surface #1e2126;"
    "@define-color umi_surface_raised #252930;"
    "@define-color umi_surface_hover #2d323a;"
    "@define-color umi_editor #191b1f;"
    "@define-color umi_border #343941;"
    "@define-color umi_border_strong #454c57;"
    "@define-color umi_foreground #e6e9ee;"
    "@define-color umi_muted #9aa3af;"
    "@define-color umi_accent #6aa9ff;"
    "@define-color umi_accent_surface #253d5d;"
    "@define-color umi_success #52c987;"
    "@define-color umi_warning #e7b85c;"
    "@define-color umi_danger #ef7777;";

static const char LIGHT_PALETTE[] =
    "@define-color umi_background #f1f3f6;"
    "@define-color umi_surface #ffffff;"
    "@define-color umi_surface_raised #f7f8fa;"
    "@define-color umi_surface_hover #e8edf4;"
    "@define-color umi_editor #ffffff;"
    "@define-color umi_border #d4d9e1;"
    "@define-color umi_border_strong #b8c0cc;"
    "@define-color umi_foreground #20242b;"
    "@define-color umi_muted #687180;"
    "@define-color umi_accent #1769c2;"
    "@define-color umi_accent_surface #dbeaff;"
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

static const char WORKBENCH_CSS[] =
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
    "  padding: 4px; border: 1px solid @umi_border_strong;"
    "  border-radius: 7px; background: @umi_surface_raised;"
    "  box-shadow: 0 8px 24px alpha(black, 0.28);"
    "}"
    ".umicom-menu-item {"
    "  min-height: 26px; padding: 4px 10px; border-radius: 4px;"
    "  color: @umi_foreground;"
    "}"
    ".umicom-menu-item:hover { background: @umi_surface_hover; }"
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
    ".umicom-toolbar-separator { margin: 5px 3px; background: @umi_border; }"
    ".umicom-workspace-profile-button > button {"
    "  min-height: 28px; padding: 3px 9px; margin-right: 4px;"
    "  border: 1px solid @umi_border_strong; border-radius: 5px;"
    "  background: @umi_surface; color: @umi_foreground; box-shadow: none;"
    "}"
    ".umicom-workspace-profile-button > button:hover {"
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
    "}"
    ".umicom-command-search {"
    "  min-height: 28px; padding: 2px 9px;"
    "  border: 1px solid @umi_border_strong; border-radius: 6px;"
    "  background: @umi_background; color: @umi_foreground;"
    "}"
    ".umicom-command-search:focus {"
    "  border-color: @umi_accent; box-shadow: 0 0 0 1px @umi_accent;"
    "}"
    ".umicom-command-results {"
    "  margin: 4px 12px 6px 52px; padding: 4px;"
    "  border: 1px solid @umi_border_strong; border-radius: 6px;"
    "  background: @umi_surface_raised; color: @umi_foreground;"
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
    "}"
    ".umicom-tool-page { padding: 0; background: @umi_surface; }"
    ".umicom-editor-area { background: @umi_editor; }"
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

static const char *palette_for_workbench(UmiUiWorkbench *workbench)
{
    UmiUiContextSnapshot context;
    if (workbench != NULL &&
        umi_ui_context_get(umi_ui_workbench_context(workbench),
                           "studio.ui.theme",
                           &context) == UMI_STATUS_OK &&
        context.kind == UMI_UI_CONTEXT_STRING) {
        if (strcmp(context.string_value, "light") == 0) {
            return LIGHT_PALETTE;
        }
        if (strcmp(context.string_value, "high-contrast") == 0) {
            return HIGH_CONTRAST_PALETTE;
        }
    }
    return DARK_PALETTE;
}

UmiStatus umi_gtk4_apply_theme(UmiGtk4Adapter *adapter,
                               UmiUiWorkbench *workbench)
{
    GdkDisplay *display;
    char *css;

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

    css = g_strconcat(palette_for_workbench(workbench), WORKBENCH_CSS, NULL);
    if (css == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    gtk_css_provider_load_from_string(adapter->theme_provider, css);
    g_free(css);
    return UMI_STATUS_OK;
}
