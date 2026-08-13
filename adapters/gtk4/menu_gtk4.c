/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/menu_gtk4.c
 *
 * PURPOSE:
 *   Render toolkit-neutral menu contributions as interactive GTK4 menu buttons
 *   and execute their canonical Framework actions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "gtk4_internal.h"

#include <ctype.h>
#include <string.h>

static void on_menu_action_clicked(GtkButton *button, gpointer user_data)
{
    UmiGtk4Adapter *adapter = (UmiGtk4Adapter *)user_data;
    const char *action_id = (const char *)g_object_get_data(
        G_OBJECT(button), "umicom-action-id");
    if (adapter == NULL || adapter->shell == NULL || action_id == NULL) return;
    umi_gtk4_dispatch_action(adapter, action_id);
}

static void menu_title(const char *menu_id, char *out_title, size_t capacity)
{
    size_t index;
    (void)g_strlcpy(out_title, menu_id, capacity);
    if (out_title[0] != '\0') {
        out_title[0] = (char)toupper((unsigned char)out_title[0]);
    }
    for (index = 0U; out_title[index] != '\0'; ++index) {
        if (out_title[index] == '-' || out_title[index] == '_') out_title[index] = ' ';
    }
}

UmiStatus umi_gtk4_refresh_menu(UmiGtk4Adapter *adapter, UmiUiWorkbench *workbench)
{
    UmiUiMenuModel *model;
    UmiUiActionModel *actions;
    size_t outer;
    if (adapter == NULL || workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_gtk4_clear_box(adapter->menu_bar);
    model = umi_ui_workbench_menus(workbench);
    actions = umi_ui_workbench_actions(workbench);
    for (outer = 0U; outer < umi_ui_menu_model_count(model); ++outer) {
        UmiUiMenuSnapshot root;
        size_t previous;
        int already_rendered = 0;
        GtkWidget *menu_button;
        GtkWidget *popover;
        GtkWidget *items_box;
        char title[UMI_UI_TEXT_CAPACITY];
        size_t inner;
        if (umi_ui_menu_model_at(model, outer, &root) != UMI_STATUS_OK) continue;
        for (previous = 0U; previous < outer; ++previous) {
            UmiUiMenuSnapshot candidate;
            if (umi_ui_menu_model_at(model, previous, &candidate) == UMI_STATUS_OK &&
                strcmp(candidate.menu_id, root.menu_id) == 0) {
                already_rendered = 1;
                break;
            }
        }
        if (already_rendered) continue;
        menu_title(root.menu_id, title, sizeof(title));
        menu_button = gtk_menu_button_new();
        gtk_menu_button_set_label(GTK_MENU_BUTTON(menu_button), title);
        popover = gtk_popover_new();
        items_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
        gtk_widget_set_margin_top(items_box, 6);
        gtk_widget_set_margin_bottom(items_box, 6);
        gtk_widget_set_margin_start(items_box, 6);
        gtk_widget_set_margin_end(items_box, 6);
        for (inner = 0U; inner < umi_ui_menu_model_count(model); ++inner) {
            UmiUiMenuSnapshot item;
            if (umi_ui_menu_model_at(model, inner, &item) != UMI_STATUS_OK ||
                strcmp(item.menu_id, root.menu_id) != 0) continue;
            if (item.separator) {
                gtk_box_append(GTK_BOX(items_box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));
            } else {
                UmiUiActionSnapshot action;
                if (umi_ui_action_model_find(actions, item.action_id, &action) == UMI_STATUS_OK && action.visible) {
                    const char *label = item.label_override[0] != '\0' ? item.label_override : action.label;
                    GtkWidget *button = gtk_button_new_with_label(label);
                    gtk_widget_set_sensitive(button, action.enabled != 0);
                    gtk_widget_set_tooltip_text(button, action.tooltip);
                    gtk_widget_set_halign(button, GTK_ALIGN_FILL);
                    g_object_set_data_full(G_OBJECT(button), "umicom-action-id",
                                           g_strdup(action.action_id), g_free);
                    g_signal_connect(button, "clicked", G_CALLBACK(on_menu_action_clicked), adapter);
                    gtk_box_append(GTK_BOX(items_box), button);
                }
            }
        }
        gtk_popover_set_child(GTK_POPOVER(popover), items_box);
        gtk_menu_button_set_popover(GTK_MENU_BUTTON(menu_button), popover);
        gtk_box_append(GTK_BOX(adapter->menu_bar), menu_button);
    }
    return UMI_STATUS_OK;
}
