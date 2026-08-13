/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/gtk4_internal.h
 *
 * PURPOSE:
 *   Share private GTK4 widget ownership and refresh helpers for the Framework reference adapter.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * GTK objects remain private to this adapter. Product-neutral headers continue to expose only opaque adapter pointers and toolkit-neutral models.
 */

#ifndef UMICOM_UI_GTK4_INTERNAL_H
#define UMICOM_UI_GTK4_INTERNAL_H

#include <gtk/gtk.h>

#include "umicom/ui/gtk4.h"
#include "umicom/ui/view_presentation.h"

struct UmiGtk4Adapter {
    GtkApplication *application;
    GtkWindow *window;
    GtkWidget *root_box;
    GtkWidget *menu_bar;
    GtkWidget *toolbar_box;

    /* Batch 23 reusable workbench chrome. */
    GtkWidget *activity_box;
    GtkWidget *sidebar_box;
    GtkWidget *sidebar_header;
    GtkWidget *breadcrumb_box;
    GtkWidget *quick_access_entry;
    GtkWidget *quick_access_list;

    GtkWidget *content_paned;
    GtkWidget *left_box;
    GtkWidget *document_notebook;
    gulong document_page_switch_handler;
    GtkWidget *right_box;
    GtkWidget *bottom_box;
    GtkWidget *status_label;
    GtkWidget *notification_label;
    UmiUiApplicationShell *shell;
};

void umi_gtk4_clear_box(GtkWidget *box);
UmiStatus umi_gtk4_build_shell(UmiGtk4Adapter *adapter);
UmiStatus umi_gtk4_refresh_workbench(UmiGtk4Adapter *adapter);
UmiStatus umi_gtk4_refresh_panes(UmiGtk4Adapter *adapter, UmiUiWorkbench *workbench);
UmiStatus umi_gtk4_build_view_widget(UmiGtk4Adapter *adapter,
                                     UmiUiWorkbench *workbench,
                                     const UmiUiPaneSnapshot *pane,
                                     GtkWidget **out_widget);
UmiStatus umi_gtk4_refresh_documents(UmiGtk4Adapter *adapter, UmiUiWorkbench *workbench);
UmiStatus umi_gtk4_refresh_menu(UmiGtk4Adapter *adapter, UmiUiWorkbench *workbench);
UmiStatus umi_gtk4_refresh_toolbar(UmiGtk4Adapter *adapter, UmiUiWorkbench *workbench);
UmiStatus umi_gtk4_refresh_status(UmiGtk4Adapter *adapter, UmiUiWorkbench *workbench);
UmiStatus umi_gtk4_refresh_notifications(UmiGtk4Adapter *adapter, UmiUiWorkbench *workbench);
UmiStatus umi_gtk4_apply_theme(UmiGtk4Adapter *adapter, UmiUiWorkbench *workbench);
UmiStatus umi_gtk4_refresh_accessibility(UmiGtk4Adapter *adapter, UmiUiWorkbench *workbench);
UmiStatus umi_gtk4_refresh_selection(UmiGtk4Adapter *adapter, UmiUiWorkbench *workbench);
UmiStatus umi_gtk4_process_dialog(UmiGtk4Adapter *adapter, UmiUiWorkbench *workbench);
void umi_gtk4_dispatch_action(UmiGtk4Adapter *adapter, const char *action_id);
GtkWidget *umi_gtk4_problems_widget(const UmiUiViewPresentation *presentation);
GtkWidget *umi_gtk4_output_widget(const UmiUiViewPresentation *presentation);
GtkWidget *umi_gtk4_terminal_widget(UmiGtk4Adapter *adapter,
                                    const UmiUiViewPresentation *presentation);
GtkWidget *umi_gtk4_process_widget(UmiGtk4Adapter *adapter,
                                   const UmiUiViewPresentation *presentation);
GtkWidget *umi_gtk4_task_widget(UmiGtk4Adapter *adapter,
                                const UmiUiViewPresentation *presentation);

/* Batch 23 chrome presenters. */
UmiStatus umi_gtk4_refresh_activity_bar(UmiGtk4Adapter *adapter,
                                       UmiUiWorkbench *workbench);
UmiStatus umi_gtk4_refresh_view_container(UmiGtk4Adapter *adapter,
                                         UmiUiWorkbench *workbench);
UmiStatus umi_gtk4_refresh_breadcrumbs(UmiGtk4Adapter *adapter,
                                      UmiUiWorkbench *workbench);
UmiStatus umi_gtk4_install_keybindings(UmiGtk4Adapter *adapter);
void umi_gtk4_on_quick_access_changed(GtkSearchEntry *entry,
                                      gpointer user_data);
void umi_gtk4_on_quick_access_row_activated(GtkListBox *list_box,
                                            GtkListBoxRow *row,
                                            gpointer user_data);

#endif
