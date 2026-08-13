/* GTK4 renderer for the Framework-owned Output view model. */
#include "gtk4_internal.h"

#include <stdio.h>

GtkWidget *umi_gtk4_output_widget(const UmiUiViewPresentation *presentation)
{
    UmiUiPropertySnapshot count_property;
    GtkWidget *scroll;
    GtkWidget *text_view;
    GtkTextBuffer *buffer;
    int64_t count = 0;
    int64_t index;
    if (presentation == NULL) return NULL;
    if (umi_ui_view_presentation_find_property(presentation, "output.count", &count_property) ==
            UMI_STATUS_OK && count_property.value.kind == UMI_UI_VALUE_INTEGER) {
        count = count_property.value.integer_value;
    }
    text_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(text_view), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(text_view), TRUE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD_CHAR);
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    for (index = 0; index < count; ++index) {
        char key[64];
        UmiUiPropertySnapshot row;
        GtkTextIter end;
        (void)snprintf(key, sizeof(key), "output.row.%lld", (long long)index);
        if (umi_ui_view_presentation_find_property(presentation, key, &row) != UMI_STATUS_OK ||
            row.value.kind != UMI_UI_VALUE_STRING) continue;
        gtk_text_buffer_get_end_iter(buffer, &end);
        gtk_text_buffer_insert(buffer, &end, row.value.string_value, -1);
        gtk_text_buffer_insert(buffer, &end, "\n", 1);
    }
    scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), text_view);
    gtk_widget_set_hexpand(scroll, TRUE);
    gtk_widget_set_vexpand(scroll, TRUE);
    return scroll;
}
