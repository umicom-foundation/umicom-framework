/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workbench_gallery_gtk4.c
 * PURPOSE:
 *   Example consumer: all text and account rows are disposable practice data. No database,
 *   payment provider, file operation or background job is started.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workbench_gallery_gtk4.c
 * Author: Sammy Hegab, Umicom Foundation | Licence: MIT
 * Example consumer: all text and account rows are disposable practice data.
 * No database, payment provider, file operation or background job is started.
 *---------------------------------------------------------------------------*/
#include "umicom/workbench_layout/viewport_gtk4.h"
#include "../../examples/workbench_viewport/practice_layout.h"
#include "desktop_system_brand.inc"
#include <stdlib.h>
#include <string.h>

static GtkWidget *Label(const char *text)
{
    GtkWidget *widget = gtk_label_new(text);
    gtk_label_set_xalign(GTK_LABEL(widget), 0);
    gtk_label_set_wrap(GTK_LABEL(widget), TRUE);
    return widget;
}

static GtkWidget *Column(void)
{
    GtkWidget *widget = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_top(widget, 12);
    gtk_widget_set_margin_bottom(widget, 12);
    gtk_widget_set_margin_start(widget, 12);
    gtk_widget_set_margin_end(widget, 12);
    return widget;
}

static void NotesChanged(GtkTextBuffer *buffer, gpointer data)
{
    char text[128];
    g_snprintf(text, sizeof text, "%d characters in this temporary note. Nothing has been saved.",
        gtk_text_buffer_get_char_count(buffer));
    gtk_label_set_text(GTK_LABEL(data), text);
}

static gboolean RowMatches(GtkListBoxRow *row, gpointer data)
{
    GtkWidget *label = gtk_list_box_row_get_child(row);
    const char *query = data;
    char *value = g_utf8_casefold(gtk_label_get_text(GTK_LABEL(label)), -1);
    gboolean match = !query[0] || strstr(value, query) != NULL;
    g_free(value);
    return match;
}

static void Search(GtkSearchEntry *entry, gpointer data)
{
    char *query = g_utf8_casefold(gtk_editable_get_text(GTK_EDITABLE(entry)), -1);
    /* Replacing the filter releases its previous owned search string. */
    gtk_list_box_set_filter_func(GTK_LIST_BOX(data), RowMatches, query, g_free);
}

static void CheckButton(GtkButton *button, gpointer data)
{
    (void)button;
    gtk_label_set_text(GTK_LABEL(data),
        "This button updated its own status label. No account, file or network changed.");
}

static void CurrencyChanged(GObject *object, GParamSpec *property, gpointer data)
{
    (void)property;
    guint choice = gtk_drop_down_get_selected(GTK_DROP_DOWN(object));
    GtkGrid *grid = GTK_GRID(data);
    for (int row = 1; row < 4; ++row) {
        bool visible = choice == 0 || (choice == 1 && row < 3) || (choice == 2 && row == 3);
        for (int column = 0; column < 3; ++column) {
            GtkWidget *cell = gtk_grid_get_child_at(grid, column, row);
            if (cell) gtk_widget_set_visible(cell, visible);
        }
    }
}

static GtkWidget *Factory(const UmiWorkbenchLayoutNode *node, void *context)
{
    (void)context;
    GtkWidget *box = Column();
    if (!strcmp(node->component_id, "gallery.navigation")) {
        gtk_box_append(GTK_BOX(box), Label("Notebook"));
        GtkWidget *search = gtk_search_entry_new();
        gtk_widget_set_tooltip_text(search,
            "Filter the practice notebook titles; this does not search your computer.");
        gtk_box_append(GTK_BOX(box), search);
        GtkWidget *list = gtk_list_box_new();
        const char *titles[] = {"Workshop plan", "Supplier questions", "Account review", "Release checklist"};
        for (size_t i = 0; i < 4; ++i) gtk_list_box_append(GTK_LIST_BOX(list), Label(titles[i]));
        gtk_box_append(GTK_BOX(box), list);
        g_signal_connect_object(search, "search-changed", G_CALLBACK(Search), list, 0);
    } else if (!strcmp(node->component_id, "gallery.notes")) {
        gtk_box_append(GTK_BOX(box), Label("Practice note - memory only"));
        GtkWidget *view = gtk_text_view_new();
        gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), GTK_WRAP_WORD_CHAR);
        gtk_widget_set_vexpand(view, TRUE);
        gtk_text_view_set_left_margin(GTK_TEXT_VIEW(view), 8);
        gtk_text_view_set_right_margin(GTK_TEXT_VIEW(view), 8);
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
        gtk_text_buffer_set_text(buffer,
            "Prepare the Umicom Notes exercise.\nTry the Account browser tab, then return here.", -1);
        gtk_box_append(GTK_BOX(box), view);
        GtkWidget *status = Label("");
        gtk_box_append(GTK_BOX(box), status);
        g_signal_connect_object(buffer, "changed", G_CALLBACK(NotesChanged), status, 0);
        NotesChanged(buffer, status);
    } else if (!strcmp(node->component_id, "gallery.accounts")) {
        gtk_box_append(GTK_BOX(box), Label("Fictional account browser - display only"));
        GtkWidget *grid = gtk_grid_new();
        gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
        gtk_grid_set_column_spacing(GTK_GRID(grid), 18);
        const char *cells[4][3] = {
            {"Account", "Currency", "Example balance"},
            {"Workshop current", "GBP", "750.00"},
            {"Workshop reserve", "GBP", "250.00"},
            {"Travel practice", "EUR", "120.00"}
        };
        for (int row = 0; row < 4; ++row) {
            for (int column = 0; column < 3; ++column) {
                gtk_grid_attach(GTK_GRID(grid), Label(cells[row][column]), column, row, 1, 1);
            }
        }
        gtk_box_append(GTK_BOX(box), grid);
        const char *choices[] = {"All currencies", "GBP", "EUR", NULL};
        GtkWidget *drop = gtk_drop_down_new_from_strings(choices);
        gtk_widget_set_tooltip_text(drop, "Filter these three fictional rows. No real account is queried or changed.");
        gtk_box_append(GTK_BOX(box), drop);
        g_signal_connect_object(drop, "notify::selected", G_CALLBACK(CurrencyChanged), grid, 0);
        GtkWidget *entry = gtk_entry_new();
        gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Temporary review comment");
        gtk_box_append(GTK_BOX(box), entry);
    } else if (!strcmp(node->component_id, "gallery.status")) {
        GtkWidget *status = Label("Use the controls to practise events and layout ownership.");
        gtk_box_append(GTK_BOX(box), status);
        GtkWidget *button = gtk_button_new_with_mnemonic("_Check this panel");
        gtk_box_append(GTK_BOX(box), button);
        g_signal_connect_object(button, "clicked", G_CALLBACK(CheckButton), status, 0);
        gtk_box_append(GTK_BOX(box), gtk_check_button_new_with_label("A local example preference"));
    } else {
        g_object_ref_sink(box);
        g_object_unref(box);
        return NULL;
    }
    return box;
}

static void Ratio(GtkRange *range, gpointer viewport)
{
    (void)UmiWorkbenchViewportWidgetSetSplit(GTK_WIDGET(viewport), "columns",
        gtk_range_get_value(range) / 100.0);
}

static void Sidebar(GtkCheckButton *button, gpointer viewport)
{
    (void)UmiWorkbenchViewportWidgetSetVisible(GTK_WIDGET(viewport), "navigation",
        gtk_check_button_get_active(button) != 0);
}

static void Another(GtkButton *button, gpointer application)
{
    (void)button;
    GtkWindow *window = UmiWorkbenchViewportGallery(GTK_APPLICATION(application));
    if (window) gtk_window_present(window);
}

GtkWindow *UmiWorkbenchViewportGallery(GtkApplication *application)
{
    if (!GTK_IS_APPLICATION(application)) return NULL;
    UmiWorkbenchLayoutDocument *document = malloc(sizeof *document);
    if (!document) return NULL;
    UmiViewportLessonCreate(document);
    GtkWidget *viewport = NULL;
    UmiStatus status = UmiWorkbenchViewportWidgetCreate(document, Factory, NULL, &viewport, NULL);
    free(document);
    if (status != UMI_STATUS_OK) return NULL;
    GtkWidget *window = gtk_application_window_new(application);
    gtk_window_set_title(GTK_WINDOW(window), "Umicom Component Workbench");
    gtk_window_set_default_size(GTK_WINDOW(window), 1100, 780);
    GtkWidget *body = Column();
    gtk_window_set_child(GTK_WINDOW(window), body);
    GBytes *bytes = g_bytes_new_static(UMICOM_SYSTEM_LOGO, sizeof UMICOM_SYSTEM_LOGO);
    GError *error = NULL;
    GdkTexture *texture = gdk_texture_new_from_bytes(bytes, &error);
    g_bytes_unref(bytes);
    if (texture) {
        GtkWidget *logo = gtk_picture_new_for_paintable(GDK_PAINTABLE(texture));
        gtk_widget_set_size_request(logo, 202, 55);
        gtk_widget_set_halign(logo, GTK_ALIGN_START);
        gtk_box_append(GTK_BOX(body), logo);
        g_object_unref(texture);
    } else {
        g_clear_error(&error);
        gtk_box_append(GTK_BOX(body), Label("Umicom"));
    }
    GtkWidget *titleRow = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GBytes *iconBytes = g_bytes_new_static(UMICOM_SYSTEM_ICON, sizeof UMICOM_SYSTEM_ICON);
    GdkTexture *iconTexture = gdk_texture_new_from_bytes(iconBytes, NULL);
    g_bytes_unref(iconBytes);
    if (iconTexture) {
        GtkWidget *mark = gtk_picture_new_for_paintable(GDK_PAINTABLE(iconTexture));
        gtk_widget_set_size_request(mark, 32, 32);
        gtk_widget_set_tooltip_text(mark, "Umicom");
        gtk_box_append(GTK_BOX(titleRow), mark);
        g_object_unref(iconTexture);
    }
    gtk_box_append(GTK_BOX(titleRow), Label("Arrange a workspace from shared components"));
    gtk_box_append(GTK_BOX(body), titleRow);
    gtk_box_append(GTK_BOX(body), Label(
        "Temporary practice data. Split and tab changes keep each panel's widgets alive. Nothing here is saved."));
    GtkWidget *widthRow = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_append(GTK_BOX(body), widthRow);
    gtk_box_append(GTK_BOX(widthRow), Label("Sidebar width (%)"));
    GtkWidget *scale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 5, 95, 1);
    gtk_range_set_value(GTK_RANGE(scale), 25);
    gtk_widget_set_hexpand(scale, TRUE);
    gtk_box_append(GTK_BOX(widthRow), scale);
    /* A second toolbar row avoids one wide, inaccessible row on small screens. */
    GtkWidget *controls = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_append(GTK_BOX(body), controls);
    GtkWidget *visible = gtk_check_button_new_with_label("Show notebook");
    gtk_check_button_set_active(GTK_CHECK_BUTTON(visible), TRUE);
    gtk_box_append(GTK_BOX(controls), visible);
    GtkWidget *second = gtk_button_new_with_label("Open another workspace");
    gtk_box_append(GTK_BOX(controls), second);
    g_signal_connect_object(scale, "value-changed", G_CALLBACK(Ratio), viewport, 0);
    g_signal_connect_object(visible, "toggled", G_CALLBACK(Sidebar), viewport, 0);
    g_signal_connect_object(second, "clicked", G_CALLBACK(Another), application, 0);
    /* The outer scroller honours the measured minimum. Smaller host windows
     * scroll instead of allowing a leaf's minimum to cover a neighbouring panel. */
    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_hexpand(scroll, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), viewport);
    gtk_box_append(GTK_BOX(body), scroll);
    gtk_box_append(GTK_BOX(body), Label(
        "Tab moves within a panel. F6 and Shift+F6 cycle visible panels. Closing discards these practice edits."));
    return GTK_WINDOW(window);
}
