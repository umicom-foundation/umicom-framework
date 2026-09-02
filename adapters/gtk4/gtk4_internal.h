/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/gtk4_internal.h
 *
 * PURPOSE:
 *   Share private GTK4 widget ownership and refresh helpers for the Framework reference adapter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * GTK objects remain private to this adapter. Product-neutral headers continue to expose only opaque adapter pointers and toolkit-neutral models.
 */

#ifndef UMICOM_UI_GTK4_INTERNAL_H
#define UMICOM_UI_GTK4_INTERNAL_H

#include <stdint.h>

#include <gtk/gtk.h>

#include "umicom/ui/gtk4.h"
#include "umicom/ui/view_presentation.h"

struct UmiGtk4Adapter {
    GtkApplication *application;
    GtkWindow *window;
    GtkWidget *root_box;
    GtkWidget *menu_bar;
    GtkWidget *toolbar_box;
    GtkWidget *toolbar_actions_box;
    GtkWidget *project_widget;
    GtkWidget *workspace_profile_button;
    GtkWidget *workspace_profile_label;
    GtkWidget *appearance_button;
    GtkWidget *appearance_label;

    /* Reusable workbench chrome. */
    GtkWidget *activity_box;
    GtkWidget *sidebar_box;
    GtkWidget *sidebar_header;
    GtkWidget *breadcrumb_box;
    GtkWidget *quick_access_entry;
    GtkWidget *quick_access_list;
    int64_t quick_access_request_seen;

    /*
     * The three splitters mirror the toolkit-neutral workbench state. Keeping
     * explicit references allows the GTK adapter to restore and persist pane
     * sizes without making Studio product code depend on GTK.
     */
    GtkWidget *content_paned;
    GtkWidget *middle_paned;
    GtkWidget *centre_paned;
    GtkWidget *left_box;
    GtkWidget *editor_paned;
    GtkWidget *document_notebook;
    gulong document_page_switch_handler;
    gulong document_page_added_handler;
    gulong document_page_reordered_handler;
    GtkWidget *secondary_document_notebook;
    gulong secondary_document_page_switch_handler;
    gulong secondary_document_page_added_handler;
    gulong secondary_document_page_reordered_handler;
    GtkWidget *right_box;
    GtkWidget *bottom_box;
    GtkWidget *desktop_layout_bar;
    GtkWidget *desktop_layout_tabs_box;
    GtkWidget *desktop_designer_button;
    GtkWidget *desktop_monitor_label;
    GtkWidget *status_box;
    GtkWidget *status_label;
    GtkWidget *notification_label;
    GtkCssProvider *theme_provider;
    GdkDisplay *theme_display;
    int applying_layout_state;
    int applying_dock_state;
    int applying_document_state;
    UmiUiApplicationShell *shell;
    UmiDesktopShellModel *desktop_shell;

    /*
     * Optional native-chrome visibility is presentation-only.  The default
     * remains the complete historical GTK4 reference adapter.  Rich hosts such
     * as Studio or Desk may explicitly configure this after presentation so the
     * adapter can provide only the central editor/workbench content while the
     * host supplies reusable Framework chrome around it.
     */
    UmiGtk4ChromeFlags chrome_visibility;
    int chrome_visibility_configured;

    /*
     * Live context-interaction capture is GTK-owned but product-neutral.
     * Signal handlers borrow this sink and are disconnected automatically with
     * the GTK objects. The sink itself is copied by value into the adapter.
     */
    UmiGtk4ContextInteractionSink context_interaction_sink;
    int context_interactions_bound;
    gulong context_primary_switch_handler;
    gulong context_secondary_switch_handler;
    uint64_t context_interaction_sequence;
};

/**
 * Provide the gtk4 clear box operation used by this module and its client applications.
 */
void umi_gtk4_clear_box(GtkWidget *box);
/**
 * Provide the gtk4 build shell operation used by this module and its client applications.
 */
UmiStatus umi_gtk4_build_shell(UmiGtk4Adapter *adapter);
/**
 * Provide the gtk4 refresh workbench operation used by this module and its client
 * applications.
 */
UmiStatus umi_gtk4_refresh_workbench(UmiGtk4Adapter *adapter);
/**
 * Provide the gtk4 refresh panes operation used by this module and its client
 * applications.
 */
UmiStatus umi_gtk4_refresh_panes(UmiGtk4Adapter *adapter, UmiUiWorkbench *workbench);
/**
 * Provide the gtk4 configure dock notebook operation used by this module and its client
 * applications.
 */
void umi_gtk4_configure_dock_notebook(UmiGtk4Adapter *adapter,
                                      GtkWidget *notebook,
                                      UmiUiPlacement placement);
/**
 * Provide the gtk4 build view widget operation used by this module and its client
 * applications.
 */
UmiStatus umi_gtk4_build_view_widget(UmiGtk4Adapter *adapter,
                                     UmiUiWorkbench *workbench,
                                     const UmiUiPaneSnapshot *pane,
                                     GtkWidget **out_widget);
/**
 * Provide the gtk4 refresh documents operation used by this module and its client
 * applications.
 */
UmiStatus umi_gtk4_refresh_documents(UmiGtk4Adapter *adapter, UmiUiWorkbench *workbench);
/**
 * Provide the gtk4 refresh menu operation used by this module and its client applications.
 */
UmiStatus umi_gtk4_refresh_menu(UmiGtk4Adapter *adapter, UmiUiWorkbench *workbench);
/**
 * Provide the gtk4 refresh toolbar operation used by this module and its client
 * applications.
 */
UmiStatus umi_gtk4_refresh_toolbar(UmiGtk4Adapter *adapter, UmiUiWorkbench *workbench);
/**
 * Provide the gtk4 refresh workspace profiles operation used by this module and its client
 * applications.
 */
UmiStatus umi_gtk4_refresh_workspace_profiles(
    UmiGtk4Adapter *adapter,
    UmiUiWorkbench *workbench);
/**
 * Provide the gtk4 refresh desktop shell operation used by this module and its client
 * applications.
 */
UmiStatus umi_gtk4_refresh_desktop_shell(UmiGtk4Adapter *adapter);
/**
 * Provide the gtk4 desktop designer popover operation used by this module and its client
 * applications.
 */
GtkWidget *umi_gtk4_desktop_designer_popover(UmiGtk4Adapter *adapter);
/**
 * Provide the gtk4 refresh appearance operation used by this module and its client
 * applications.
 */
UmiStatus umi_gtk4_refresh_appearance(UmiGtk4Adapter *adapter,
                                     UmiUiWorkbench *workbench);
/**
 * Provide the gtk4 refresh status operation used by this module and its client
 * applications.
 */
UmiStatus umi_gtk4_refresh_status(UmiGtk4Adapter *adapter, UmiUiWorkbench *workbench);
/**
 * Provide the gtk4 refresh notifications operation used by this module and its client
 * applications.
 */
UmiStatus umi_gtk4_refresh_notifications(UmiGtk4Adapter *adapter, UmiUiWorkbench *workbench);
/**
 * Provide the gtk4 apply theme operation used by this module and its client applications.
 */
UmiStatus umi_gtk4_apply_theme(UmiGtk4Adapter *adapter, UmiUiWorkbench *workbench);
/**
 * Provide the gtk4 configure editor theme operation used by this module and its client
 * applications.
 */
UmiStatus umi_gtk4_configure_editor_theme(GtkWidget *editor,
                                          UmiUiWorkbench *workbench);
/**
 * Provide the gtk4 apply editor themes operation used by this module and its client
 * applications.
 */
UmiStatus umi_gtk4_apply_editor_themes(UmiGtk4Adapter *adapter,
                                       UmiUiWorkbench *workbench);
/**
 * Provide the gtk4 refresh accessibility operation used by this module and its client
 * applications.
 */
UmiStatus umi_gtk4_refresh_accessibility(UmiGtk4Adapter *adapter, UmiUiWorkbench *workbench);
/**
 * Provide the gtk4 refresh selection operation used by this module and its client
 * applications.
 */
UmiStatus umi_gtk4_refresh_selection(UmiGtk4Adapter *adapter, UmiUiWorkbench *workbench);
/**
 * Provide the gtk4 process dialog operation used by this module and its client
 * applications.
 */
UmiStatus umi_gtk4_process_dialog(UmiGtk4Adapter *adapter, UmiUiWorkbench *workbench);
/**
 * Provide the gtk4 dispatch action operation used by this module and its client
 * applications.
 */
void umi_gtk4_dispatch_action(UmiGtk4Adapter *adapter, const char *action_id);
/**
 * Provide the gtk4 problems widget operation used by this module and its client
 * applications.
 */
GtkWidget *umi_gtk4_problems_widget(const UmiUiViewPresentation *presentation);
/**
 * Provide the gtk4 output widget operation used by this module and its client
 * applications.
 */
GtkWidget *umi_gtk4_output_widget(const UmiUiViewPresentation *presentation);
/**
 * Provide the gtk4 terminal widget operation used by this module and its client
 * applications.
 */
GtkWidget *umi_gtk4_terminal_widget(UmiGtk4Adapter *adapter,
                                    const UmiUiViewPresentation *presentation);
/**
 * Provide the gtk4 process widget operation used by this module and its client
 * applications.
 */
GtkWidget *umi_gtk4_process_widget(UmiGtk4Adapter *adapter,
                                   const UmiUiViewPresentation *presentation);
/**
 * Provide the gtk4 task widget operation used by this module and its client applications.
 */
GtkWidget *umi_gtk4_task_widget(UmiGtk4Adapter *adapter,
                                const UmiUiViewPresentation *presentation);
/**
 * Provide the gtk4 source control widget operation used by this module and its client
 * applications.
 */
GtkWidget *umi_gtk4_source_control_widget(
    UmiGtk4Adapter *adapter,
    const UmiUiViewPresentation *presentation);

/* Reusable workbench chrome presenters. */
UmiStatus umi_gtk4_refresh_activity_bar(UmiGtk4Adapter *adapter,
                                       UmiUiWorkbench *workbench);
/**
 * Provide the gtk4 refresh view container operation used by this module and its client
 * applications.
 */
UmiStatus umi_gtk4_refresh_view_container(UmiGtk4Adapter *adapter,
                                         UmiUiWorkbench *workbench);
/**
 * Provide the gtk4 refresh breadcrumbs operation used by this module and its client
 * applications.
 */
UmiStatus umi_gtk4_refresh_breadcrumbs(UmiGtk4Adapter *adapter,
                                      UmiUiWorkbench *workbench);
/**
 * Provide the gtk4 install keybindings operation used by this module and its client
 * applications.
 */
UmiStatus umi_gtk4_install_keybindings(UmiGtk4Adapter *adapter);
/**
 * Provide the gtk4 on quick access changed operation used by this module and its client
 * applications.
 */
void umi_gtk4_on_quick_access_changed(GtkSearchEntry *entry,
                                      gpointer user_data);
/**
 * Provide the gtk4 on quick access activate operation used by this module and its client
 * applications.
 */
void umi_gtk4_on_quick_access_activate(GtkSearchEntry *entry,
                                       gpointer user_data);
/**
 * Provide the gtk4 on quick access row activated operation used by this module and its
 * client applications.
 */
void umi_gtk4_on_quick_access_row_activated(GtkListBox *list_box,
                                            GtkListBoxRow *row,
                                            gpointer user_data);
/**
 * Provide the gtk4 refresh quick access request operation used by this module and its
 * client applications.
 */
void umi_gtk4_refresh_quick_access_request(UmiGtk4Adapter *adapter,
                                           UmiUiWorkbench *workbench);

/* Live native-interaction binding. */
UmiStatus umi_gtk4_context_interaction_refresh(
    UmiGtk4Adapter *adapter);
/**
 * Provide the gtk4 context interaction unbind operation used by this module and its client
 * applications.
 */
void umi_gtk4_context_interaction_unbind(
    UmiGtk4Adapter *adapter);
/**
 * Provide the gtk4 context interaction tag problem list operation used by this module and
 * its client applications.
 */
void umi_gtk4_context_interaction_tag_problem_list(
    GtkWidget *list);
/**
 * Provide the gtk4 context interaction tag source control row operation used by this
 * module and its client applications.
 */
void umi_gtk4_context_interaction_tag_source_control_row(
    GtkWidget *widget,
    const char *view_kind,
    const char *row_text);

#endif
