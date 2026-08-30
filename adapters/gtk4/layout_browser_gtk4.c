/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/layout_browser_gtk4.c
 *
 * PURPOSE:
 *   Render the Framework Layout Browser model with search, category, list,
 *   preview and action regions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "workbench_designer_gtk4_internal.h"


static void browser_search_changed(GtkEditable *editable, gpointer user_data)
{
    UmiWorkbenchLayoutBrowserGtk4 *browser = user_data;
    const char *text;
    if (browser == NULL) return;
    text = gtk_editable_get_text(editable);
    (void)text;
    browser->revision += 1U;
}

static void browser_open_clicked(GtkButton *button, gpointer user_data)
{
    UmiWorkbenchLayoutBrowserGtk4 *browser = user_data;
    (void)button;
    if (browser == NULL || browser->controller == NULL) return;
    browser->revision += 1U;
}

UmiStatus umi_workbench_layout_browser_gtk4_create(
    UmiWorkbenchDesignerController *controller,
    UmiWorkbenchLayoutBrowserGtk4 **out_browser)
{
    UmiWorkbenchLayoutBrowserGtk4 *browser;
    GtkWidget *toolbar;
    GtkWidget *paned;
    GtkWidget *list_scrolled;
    GtkWidget *preview_box;
    GtkWidget *actions;
    if (controller == NULL || out_browser == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_browser = NULL;
    browser = g_new0(UmiWorkbenchLayoutBrowserGtk4, 1U);
    if (browser == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    browser->controller = controller;
    umi_workbench_designer_browser_init(&browser->model);
    browser->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_add_css_class(browser->root, "umicom-layout-browser");
    toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    browser->search_entry = gtk_search_entry_new();
    gtk_widget_set_hexpand(browser->search_entry, TRUE);
    gtk_editable_set_text(GTK_EDITABLE(browser->search_entry), "");
    g_signal_connect(
        browser->search_entry, "changed",
        G_CALLBACK(browser_search_changed), browser);
    browser->category_combo = gtk_drop_down_new_from_strings(
        (const char *const[]){"All", "Development", "Trading", "Operations", NULL});
    gtk_box_append(GTK_BOX(toolbar), browser->search_entry);
    gtk_box_append(GTK_BOX(toolbar), browser->category_combo);
    gtk_box_append(GTK_BOX(browser->root), toolbar);

    paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    browser->list_box = gtk_list_box_new();
    gtk_list_box_set_selection_mode(
        GTK_LIST_BOX(browser->list_box), GTK_SELECTION_SINGLE);
    list_scrolled = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(
        GTK_SCROLLED_WINDOW(list_scrolled), browser->list_box);
    gtk_widget_set_size_request(list_scrolled, 360, 220);
    gtk_paned_set_start_child(GTK_PANED(paned), list_scrolled);

    preview_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    browser->preview_frame = gtk_frame_new("Layout Preview");
    browser->preview_area = gtk_drawing_area_new();
    gtk_drawing_area_set_content_width(
        GTK_DRAWING_AREA(browser->preview_area), 360);
    gtk_drawing_area_set_content_height(
        GTK_DRAWING_AREA(browser->preview_area), 220);
    gtk_drawing_area_set_draw_func(
        GTK_DRAWING_AREA(browser->preview_area),
        umi_workbench_layout_browser_gtk4_preview_draw,
        browser,
        NULL);
    gtk_frame_set_child(
        GTK_FRAME(browser->preview_frame), browser->preview_area);
    browser->name_label = gtk_label_new("No layout selected");
    gtk_label_set_xalign(GTK_LABEL(browser->name_label), 0.0F);
    browser->description_label = gtk_label_new("");
    gtk_label_set_wrap(GTK_LABEL(browser->description_label), TRUE);
    gtk_label_set_xalign(GTK_LABEL(browser->description_label), 0.0F);
    actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    browser->open_button = gtk_button_new_with_label("Open");
    browser->clone_button = gtk_button_new_with_label("Clone");
    browser->delete_button = gtk_button_new_with_label("Delete");
    g_signal_connect(
        browser->open_button, "clicked",
        G_CALLBACK(browser_open_clicked), browser);
    gtk_box_append(GTK_BOX(actions), browser->open_button);
    gtk_box_append(GTK_BOX(actions), browser->clone_button);
    gtk_box_append(GTK_BOX(actions), browser->delete_button);
    gtk_box_append(GTK_BOX(preview_box), browser->preview_frame);
    gtk_box_append(GTK_BOX(preview_box), browser->name_label);
    gtk_box_append(GTK_BOX(preview_box), browser->description_label);
    gtk_box_append(GTK_BOX(preview_box), actions);
    gtk_paned_set_end_child(GTK_PANED(paned), preview_box);
    gtk_box_append(GTK_BOX(browser->root), paned);
    *out_browser = browser;
    return UMI_STATUS_OK;
}

void umi_workbench_layout_browser_gtk4_destroy(
    UmiWorkbenchLayoutBrowserGtk4 *browser)
{
    if (browser != NULL) g_free(browser);
}

GtkWidget *umi_workbench_layout_browser_gtk4_widget(
    UmiWorkbenchLayoutBrowserGtk4 *browser)
{
    return browser != NULL ? browser->root : NULL;
}

UmiStatus umi_workbench_layout_browser_gtk4_refresh(
    UmiWorkbenchLayoutBrowserGtk4 *browser,
    const UmiWorkbenchDesignerBrowser *model)
{
    size_t index;
    GtkWidget *child;
    if (browser == NULL || model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    browser->model = *model;
    child = gtk_widget_get_first_child(browser->list_box);
    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_list_box_remove(GTK_LIST_BOX(browser->list_box), child);
        child = next;
    }
    for (index = 0U; index < model->count; ++index) {
        const UmiWorkbenchDesignerBrowserItem *item = &model->items[index];
        GtkWidget *row_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
        GtkWidget *name = gtk_label_new(item->name);
        GtkWidget *meta = gtk_label_new(item->category);
        gtk_label_set_xalign(GTK_LABEL(name), 0.0F);
        gtk_label_set_xalign(GTK_LABEL(meta), 0.0F);
        gtk_widget_add_css_class(meta, "dim-label");
        gtk_box_append(GTK_BOX(row_box), name);
        gtk_box_append(GTK_BOX(row_box), meta);
        gtk_list_box_append(GTK_LIST_BOX(browser->list_box), row_box);
    }
    gtk_widget_queue_draw(browser->preview_area);
    browser->revision += 1U;
    return UMI_STATUS_OK;
}
