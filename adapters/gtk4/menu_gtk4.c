/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/menu_gtk4.c
 *
 * PURPOSE:
 *   Render toolkit-neutral menu contributions as interactive GTK4 menu buttons
 *   and execute their canonical Framework actions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "gtk4_internal.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

static void on_menu_action_clicked(GtkButton *button, gpointer user_data)
{
    UmiGtk4Adapter *adapter = (UmiGtk4Adapter *)user_data;
    const char *action_id = (const char *)g_object_get_data(
        G_OBJECT(button), "umicom-action-id");
    GtkWidget *menu_button = (GtkWidget *)g_object_get_data(
        G_OBJECT(button), "umicom-menu-button");
    if (adapter == NULL || adapter->shell == NULL || action_id == NULL) return;
    if (menu_button != NULL && GTK_IS_MENU_BUTTON(menu_button)) {
        gtk_menu_button_popdown(GTK_MENU_BUTTON(menu_button));
    }
    umi_gtk4_dispatch_action(adapter, action_id);
}

static void menu_title(const char *menu_id, char *out_title, size_t capacity)
{
    size_t index;
    if (strcmp(menu_id, "source-control") == 0) {
        (void)g_strlcpy(out_title, "VCS", capacity);
        return;
    }
    if (strcmp(menu_id, "go") == 0) {
        (void)g_strlcpy(out_title, "Navigate", capacity);
        return;
    }
    if (strcmp(menu_id, "ai") == 0) {
        (void)g_strlcpy(out_title, "AI", capacity);
        return;
    }
    (void)g_strlcpy(out_title, menu_id, capacity);
    if (out_title[0] != '\0') {
        out_title[0] = (char)toupper((unsigned char)out_title[0]);
    }
    for (index = 0U; out_title[index] != '\0'; ++index) {
        if (out_title[index] == '-' || out_title[index] == '_') out_title[index] = ' ';
    }
}

static int compare_menu_items(const void *left, const void *right)
{
    const UmiUiMenuSnapshot *first = (const UmiUiMenuSnapshot *)left;
    const UmiUiMenuSnapshot *second = (const UmiUiMenuSnapshot *)right;
    if (first->order < second->order) return -1;
    if (first->order > second->order) return 1;
    return strcmp(first->item_id, second->item_id);
}

static GtkWidget *create_menu_item(UmiGtk4Adapter *adapter,
                                   GtkWidget *menu_button,
                                   const UmiUiMenuSnapshot *item,
                                   const UmiUiActionSnapshot *action)
{
    const char *label_text = item->label_override[0] != '\0'
        ? item->label_override : action->label;
    GtkWidget *button = gtk_button_new();
    GtkWidget *content = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *state = action->checkable && action->checked
        ? gtk_image_new_from_icon_name("object-select-symbolic")
        : gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget *icon = gtk_image_new_from_icon_name(action->icon_name);
    GtkWidget *label = gtk_label_new(label_text);
    GtkWidget *accelerator = gtk_label_new(action->accelerator);

    gtk_widget_set_size_request(state, 16, 16);
    gtk_widget_add_css_class(state, "umicom-menu-state");
    gtk_image_set_pixel_size(GTK_IMAGE(icon), 16);
    gtk_widget_set_size_request(icon, 16, 16);
    gtk_widget_add_css_class(icon, "umicom-menu-icon");
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_label_set_ellipsize(GTK_LABEL(label), PANGO_ELLIPSIZE_END);
    gtk_widget_set_hexpand(label, TRUE);
    gtk_widget_add_css_class(label, "umicom-menu-label");
    gtk_label_set_xalign(GTK_LABEL(accelerator), 1.0F);
    gtk_widget_add_css_class(accelerator, "umicom-menu-accelerator");

    gtk_box_append(GTK_BOX(content), state);
    gtk_box_append(GTK_BOX(content), icon);
    gtk_box_append(GTK_BOX(content), label);
    gtk_box_append(GTK_BOX(content), accelerator);
    gtk_button_set_child(GTK_BUTTON(button), content);
    gtk_widget_set_sensitive(button, action->enabled != 0);
    gtk_widget_set_tooltip_text(button, action->tooltip);
    gtk_widget_set_halign(button, GTK_ALIGN_FILL);
    gtk_widget_add_css_class(button, "flat");
    gtk_widget_add_css_class(button, "umicom-menu-item");
    if (action->checkable && action->checked) {
        gtk_widget_add_css_class(button, "checked");
    }
    g_object_set_data_full(G_OBJECT(button), "umicom-action-id",
                           g_strdup(action->action_id), g_free);
    g_object_set_data(G_OBJECT(button), "umicom-menu-button", menu_button);
    g_signal_connect(button, "clicked", G_CALLBACK(on_menu_action_clicked),
                     adapter);
    return button;
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
        UmiUiMenuSnapshot items[UMI_UI_MENU_MAX];
        size_t item_count = 0U;
        size_t inner;
        char previous_section[UMI_UI_ID_CAPACITY] = "";
        int has_item = 0;
        int last_was_separator = 0;
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
        gtk_widget_add_css_class(menu_button, "umicom-menu-button");
        popover = gtk_popover_new();
        gtk_widget_add_css_class(popover, "umicom-menu-popover");
        items_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
        gtk_widget_add_css_class(items_box, "umicom-menu-items");
        gtk_widget_set_margin_top(items_box, 6);
        gtk_widget_set_margin_bottom(items_box, 6);
        gtk_widget_set_margin_start(items_box, 6);
        gtk_widget_set_margin_end(items_box, 6);
        for (inner = 0U; inner < umi_ui_menu_model_count(model); ++inner) {
            UmiUiMenuSnapshot item;
            if (umi_ui_menu_model_at(model, inner, &item) != UMI_STATUS_OK ||
                strcmp(item.menu_id, root.menu_id) != 0) continue;
            if (item_count < UMI_UI_MENU_MAX) items[item_count++] = item;
        }
        qsort(items, item_count, sizeof(items[0]), compare_menu_items);
        for (inner = 0U; inner < item_count; ++inner) {
            const UmiUiMenuSnapshot *item = &items[inner];
            if (item->separator) {
                if (has_item && !last_was_separator) {
                    GtkWidget *separator = gtk_separator_new(
                        GTK_ORIENTATION_HORIZONTAL);
                    gtk_widget_add_css_class(separator,
                                             "umicom-menu-separator");
                    gtk_box_append(GTK_BOX(items_box), separator);
                    last_was_separator = 1;
                }
            } else {
                UmiUiActionSnapshot action;
                if (umi_ui_action_model_find(actions, item->action_id,
                                             &action) == UMI_STATUS_OK &&
                    action.visible) {
                    if (has_item && !last_was_separator &&
                        previous_section[0] != '\0' &&
                        item->section_id[0] != '\0' &&
                        strcmp(previous_section, item->section_id) != 0) {
                        GtkWidget *separator = gtk_separator_new(
                            GTK_ORIENTATION_HORIZONTAL);
                        gtk_widget_add_css_class(separator,
                                                 "umicom-menu-separator");
                        gtk_box_append(GTK_BOX(items_box), separator);
                    }
                    gtk_box_append(GTK_BOX(items_box),
                                   create_menu_item(adapter, menu_button,
                                                    item, &action));
                    (void)g_strlcpy(previous_section, item->section_id,
                                    sizeof(previous_section));
                    has_item = 1;
                    last_was_separator = 0;
                }
            }
        }
        gtk_popover_set_child(GTK_POPOVER(popover), items_box);
        gtk_menu_button_set_popover(GTK_MENU_BUTTON(menu_button), popover);
        gtk_box_append(GTK_BOX(adapter->menu_bar), menu_button);
    }
    return UMI_STATUS_OK;
}
