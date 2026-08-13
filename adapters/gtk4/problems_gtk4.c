/* GTK4 renderer for the Framework-owned Problems view model. */
#include "gtk4_internal.h"

#include <stdio.h>

static int64_t integer_property(const UmiUiViewPresentation *presentation,
                                const char *key)
{
    UmiUiPropertySnapshot property;
    if (umi_ui_view_presentation_find_property(presentation, key, &property) == UMI_STATUS_OK &&
        property.value.kind == UMI_UI_VALUE_INTEGER) return property.value.integer_value;
    return 0;
}

GtkWidget *umi_gtk4_problems_widget(const UmiUiViewPresentation *presentation)
{
    GtkWidget *root;
    GtkWidget *summary;
    GtkWidget *list;
    GtkWidget *scroll;
    char summary_text[192];
    int64_t count;
    int64_t index;
    if (presentation == NULL) return NULL;
    root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_widget_set_margin_top(root, 6);
    gtk_widget_set_margin_bottom(root, 6);
    gtk_widget_set_margin_start(root, 6);
    gtk_widget_set_margin_end(root, 6);
    (void)snprintf(summary_text, sizeof(summary_text),
                   "%lld errors  %lld warnings  %lld active",
                   (long long)integer_property(presentation, "problem.errors"),
                   (long long)integer_property(presentation, "problem.warnings"),
                   (long long)integer_property(presentation, "problem.active"));
    summary = gtk_label_new(summary_text);
    gtk_label_set_xalign(GTK_LABEL(summary), 0.0F);
    gtk_widget_add_css_class(summary, "heading");
    gtk_box_append(GTK_BOX(root), summary);
    list = gtk_list_box_new();
    gtk_list_box_set_selection_mode(GTK_LIST_BOX(list), GTK_SELECTION_SINGLE);
    count = integer_property(presentation, "problem.count");
    for (index = 0; index < count; ++index) {
        char key[64];
        UmiUiPropertySnapshot row;
        GtkWidget *label;
        (void)snprintf(key, sizeof(key), "problem.row.%lld", (long long)index);
        if (umi_ui_view_presentation_find_property(presentation, key, &row) != UMI_STATUS_OK ||
            row.value.kind != UMI_UI_VALUE_STRING) continue;
        label = gtk_label_new(row.value.string_value);
        gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
        gtk_label_set_wrap(GTK_LABEL(label), TRUE);
        gtk_widget_set_margin_top(label, 4);
        gtk_widget_set_margin_bottom(label, 4);
        gtk_widget_set_margin_start(label, 6);
        gtk_widget_set_margin_end(label, 6);
        gtk_list_box_append(GTK_LIST_BOX(list), label);
    }
    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), list);
    gtk_widget_set_hexpand(scroll, TRUE);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_box_append(GTK_BOX(root), scroll);
    return root;
}
