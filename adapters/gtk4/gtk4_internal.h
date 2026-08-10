/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/gtk4_internal.h
 *
 * PURPOSE:
 *   Share private GTK4 widget ownership and component-refresh helpers between the
 *   small reference-adapter implementation units.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_GTK4_INTERNAL_H
#define UMICOM_UI_GTK4_INTERNAL_H

#include <gtk/gtk.h>

#include "umicom/ui/gtk4.h"

struct UmiGtk4Adapter {
    GtkApplication *application;
    GtkWindow *window;
    GtkWidget *root_box;
    GtkWidget *menu_bar;
    GtkWidget *toolbar_box;
    GtkWidget *content_paned;
    GtkWidget *left_box;
    GtkWidget *document_notebook;
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
UmiStatus umi_gtk4_refresh_documents(UmiGtk4Adapter *adapter, UmiUiWorkbench *workbench);
UmiStatus umi_gtk4_refresh_menu(UmiGtk4Adapter *adapter, UmiUiWorkbench *workbench);
UmiStatus umi_gtk4_refresh_toolbar(UmiGtk4Adapter *adapter, UmiUiWorkbench *workbench);
UmiStatus umi_gtk4_refresh_status(UmiGtk4Adapter *adapter, UmiUiWorkbench *workbench);
UmiStatus umi_gtk4_refresh_notifications(UmiGtk4Adapter *adapter, UmiUiWorkbench *workbench);
UmiStatus umi_gtk4_apply_theme(UmiGtk4Adapter *adapter, UmiUiWorkbench *workbench);
UmiStatus umi_gtk4_refresh_accessibility(UmiGtk4Adapter *adapter, UmiUiWorkbench *workbench);
UmiStatus umi_gtk4_refresh_selection(UmiGtk4Adapter *adapter, UmiUiWorkbench *workbench);
UmiStatus umi_gtk4_process_dialog(UmiGtk4Adapter *adapter, UmiUiWorkbench *workbench);

#endif
