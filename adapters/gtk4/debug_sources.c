/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/debug_sources.c
 * PURPOSE: Browse copied debugger source descriptors and show fetched text without local-file side effects.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/debug_sources.h"
#include "umicom/ui/gtk4/automation.h"
#include <limits.h>
#include <string.h>
enum
{
    SOURCE_ROWS = 16
};
typedef struct SourcePanel
{
    GtkWidget *root, *rows, *status, *search, *previous, *next, *viewer, *content_status;
    UmiDebugSourceCatalog catalog;
    size_t first;
    gboolean valid, pending;
    UmiGtk4DebugSourcesRefresh refresh;
    UmiGtk4DebugSourcesRead read;
    void *context;
    GDestroyNotify destroy;
} SourcePanel;
static void SourcePanelFree(gpointer data)
{
    SourcePanel *panel = data;
    if (panel->destroy != NULL)
        panel->destroy(panel->context);
    g_free(panel);
}
static bool SourceTextValid(const char *text, size_t capacity)
{
    return memchr(text, '\0', capacity) != NULL && g_utf8_validate(text, -1, NULL);
}
static bool SourceCatalogValid(const UmiDebugSourceCatalog *catalog)
{
    if (catalog->count > UMI_DEBUG_SOURCE_CATALOG_LIMIT || catalog->connection.generation == 0U ||
        !SourceTextValid(catalog->connection.session_id, sizeof catalog->connection.session_id) ||
        catalog->connection.session_id[0] == '\0')
        return false;
    for (size_t i = 0U; i < catalog->count; ++i)
    {
        const UmiDebugSourceRecord *item = &catalog->items[i];
        if (item->reference > INT32_MAX || !SourceTextValid(item->name, sizeof item->name) ||
            !SourceTextValid(item->path, sizeof item->path) ||
            !SourceTextValid(item->origin, sizeof item->origin) ||
            !SourceTextValid(item->presentation_hint, sizeof item->presentation_hint))
            return false;
    }
    return true;
}
static void SourceRowsClear(SourcePanel *panel)
{
    GtkWidget *child;
    while ((child = gtk_widget_get_first_child(panel->rows)) != NULL)
        gtk_box_remove(GTK_BOX(panel->rows), child);
    gtk_widget_set_sensitive(panel->previous, FALSE);
    gtk_widget_set_sensitive(panel->next, FALSE);
}
static void SourceContentClear(SourcePanel *panel)
{
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->viewer)), "", 0);
    gtk_label_set_text(GTK_LABEL(panel->content_status),
                       "Choose View source beside an adapter reference.");
}
static bool SourceMatches(const UmiDebugSourceRecord *item, const char *query)
{
    if (query[0] == '\0')
        return true;
    char *joined = g_strconcat(item->name, "\n", item->path, "\n", item->origin, NULL);
    char *folded = g_utf8_casefold(joined, -1);
    g_free(joined);
    bool match = strstr(folded, query) != NULL;
    g_free(folded);
    return match;
}
static void SourceView(GtkButton *button, gpointer data)
{
    SourcePanel *panel = g_object_get_data(G_OBJECT(data), "umicom-source-panel");
    guint encoded = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(button), "umicom-source-index"));
    if (panel->pending || !panel->valid || !gtk_widget_get_mapped(panel->root) ||
        !gtk_widget_get_mapped(GTK_WIDGET(button)) ||
        !gtk_widget_is_ancestor(GTK_WIDGET(button), panel->rows) || encoded == 0U)
        return;
    size_t index = (size_t)(encoded - 1U);
    if (index >= panel->catalog.count || panel->catalog.items[index].reference == 0U)
        return;
    /* Copy the selected identity before calling a host that may close this window.
     * The strong root hold protects the copied catalogue and response widgets. */
    GtkWidget *root = g_object_ref(panel->root);
    panel->pending = TRUE;
    SourceContentClear(panel);
    UmiDebugSourceContent *content = g_new0(UmiDebugSourceContent, 1);
    UmiStatus status = panel->read(panel->context, &panel->catalog.connection,
                                   panel->catalog.items[index].reference, content);
    if (status == UMI_STATUS_OK &&
        (!SourceTextValid(content->text, sizeof content->text) ||
         !SourceTextValid(content->mime_type, sizeof content->mime_type) ||
         content->bytes != strlen(content->text)))
        status = UMI_STATUS_INVALID_ARGUMENT;
    if (status == UMI_STATUS_OK)
    {
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->viewer)),
                                 content->text, (int)content->bytes);
        const UmiDebugSourceRecord *item = &panel->catalog.items[index];
        char *summary =
            g_strdup_printf("%s — reference %u; %zu UTF-8 bytes; %s. Read-only adapter response.",
                            item->name[0] ? item->name : "Source", item->reference, content->bytes,
                            content->mime_type[0] ? content->mime_type : "MIME type not reported");
        gtk_label_set_text(GTK_LABEL(panel->content_status), summary);
        g_free(summary);
    }
    else
        gtk_label_set_text(GTK_LABEL(panel->content_status), umi_status_text(status));
    g_free(content);
    panel->pending = FALSE;
    g_object_unref(root);
}
static void SourceRender(SourcePanel *panel)
{
    SourceRowsClear(panel);
    if (!panel->valid)
        return;
    const char *text = gtk_editable_get_text(GTK_EDITABLE(panel->search));
    char *query = g_utf8_casefold(text, -1);
    size_t matched = 0U, shown = 0U;
    for (size_t i = 0U; i < panel->catalog.count; ++i)
    {
        const UmiDebugSourceRecord *item = &panel->catalog.items[i];
        if (!SourceMatches(item, query))
            continue;
        size_t position = matched++;
        if (position < panel->first || shown >= SOURCE_ROWS)
            continue;
        ++shown;
        GtkWidget *row = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
        char *label =
            g_strdup_printf("%s\n%s\nOrigin: %s; related sources: %zu",
                            item->name[0] ? item->name : "Unnamed source",
                            item->path[0] ? item->path : "Path not reported",
                            item->origin[0] ? item->origin : "Not reported", item->related_count);
        GtkWidget *description = gtk_label_new(label);
        g_free(label);
        gtk_label_set_xalign(GTK_LABEL(description), 0.0F);
        gtk_label_set_wrap(GTK_LABEL(description), TRUE);
        gtk_label_set_selectable(GTK_LABEL(description), TRUE);
        gtk_box_append(GTK_BOX(row), description);
        GtkWidget *view = gtk_button_new_with_label(
            item->reference ? "View source" : "No adapter source reference");
        gtk_widget_set_sensitive(view, item->reference != 0U);
        char tag[80];
        g_snprintf(tag, sizeof tag, "debug.sources.view.%zu", i);
        (void)umi_gtk4_automation_tag_widget(view, tag);
        g_object_set_data(G_OBJECT(view), "umicom-source-index", GUINT_TO_POINTER((guint)i + 1U));
        g_signal_connect_object(view, "clicked", G_CALLBACK(SourceView), panel->root, 0);
        gtk_box_append(GTK_BOX(row), view);
        gtk_box_append(GTK_BOX(panel->rows), row);
    }
    g_free(query);
    char *summary = g_strdup_printf(
        "%zu of %zu sources match; %zu shown. Catalogue remains captured until Refresh.", matched,
        panel->catalog.count, shown);
    gtk_label_set_text(GTK_LABEL(panel->status), summary);
    g_free(summary);
    gtk_widget_set_sensitive(panel->previous, panel->first != 0U);
    gtk_widget_set_sensitive(panel->next, panel->first + shown < matched);
}
static void SourceRefresh(GtkButton *button, gpointer data)
{
    (void)button;
    SourcePanel *panel = g_object_get_data(G_OBJECT(data), "umicom-source-panel");
    if (panel->pending || !gtk_widget_get_mapped(panel->root))
        return;
    GtkWidget *root = g_object_ref(panel->root);
    panel->pending = TRUE;
    /* A failed replacement must not leave an earlier connection's source rows
     * looking current. The independent local document editor is untouched. */
    panel->valid = FALSE;
    panel->first = 0U;
    SourceRowsClear(panel);
    SourceContentClear(panel);
    UmiDebugSourceCatalog *catalog = g_new0(UmiDebugSourceCatalog, 1);
    UmiStatus status = panel->refresh(panel->context, catalog);
    if (status == UMI_STATUS_OK && !SourceCatalogValid(catalog))
        status = UMI_STATUS_INVALID_ARGUMENT;
    if (status == UMI_STATUS_OK)
    {
        panel->catalog = *catalog;
        panel->valid = TRUE;
        SourceRender(panel);
    }
    else
        gtk_label_set_text(GTK_LABEL(panel->status),
                           status == UMI_STATUS_NOT_IMPLEMENTED
                               ? "This adapter did not advertise loaded source listing."
                               : umi_status_text(status));
    g_free(catalog);
    panel->pending = FALSE;
    g_object_unref(root);
}
static void SourceSearch(GtkEditable *editable, gpointer data)
{
    (void)editable;
    SourcePanel *panel = g_object_get_data(G_OBJECT(data), "umicom-source-panel");
    if (panel->pending)
        return;
    panel->first = 0U;
    SourceRender(panel);
}
static void SourcePrevious(GtkButton *button, gpointer data)
{
    (void)button;
    SourcePanel *panel = g_object_get_data(G_OBJECT(data), "umicom-source-panel");
    if (panel->pending || !panel->valid || !gtk_widget_get_mapped(panel->root) ||
        panel->first == 0U)
        return;
    panel->first = panel->first > SOURCE_ROWS ? panel->first - SOURCE_ROWS : 0U;
    SourceRender(panel);
}
static void SourceNext(GtkButton *button, gpointer data)
{
    (void)button;
    SourcePanel *panel = g_object_get_data(G_OBJECT(data), "umicom-source-panel");
    if (panel->pending || !panel->valid || !gtk_widget_get_mapped(panel->root) ||
        !gtk_widget_get_sensitive(panel->next))
        return;
    panel->first += SOURCE_ROWS;
    SourceRender(panel);
}
GtkWidget *UmiGtk4DebugSourcesCreate(UmiGtk4DebugSourcesRefresh refresh,
                                     UmiGtk4DebugSourcesRead read, void *context,
                                     GDestroyNotify destroy)
{
    if (refresh == NULL || read == NULL)
        return NULL;
    SourcePanel *panel = g_new0(SourcePanel, 1);
    panel->refresh = refresh;
    panel->read = read;
    panel->context = context;
    panel->destroy = destroy;
    panel->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    panel->rows = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    panel->status = gtk_label_new("Start or attach a debugger, then choose Refresh sources.");
    panel->content_status = gtk_label_new("Choose View source beside an adapter reference.");
    gtk_label_set_wrap(GTK_LABEL(panel->status), TRUE);
    gtk_label_set_xalign(GTK_LABEL(panel->status), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(panel->content_status), TRUE);
    gtk_label_set_xalign(GTK_LABEL(panel->content_status), 0.0F);
    GtkWidget *explanation =
        gtk_label_new("Source paths may refer to another machine. This panel reads positive "
                      "adapter references only; open ordinary project files through the editor.");
    gtk_label_set_wrap(GTK_LABEL(explanation), TRUE);
    gtk_label_set_xalign(GTK_LABEL(explanation), 0.0F);
    GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *refresh_button = gtk_button_new_with_label("Refresh sources");
    panel->previous = gtk_button_new_with_label("Previous");
    panel->next = gtk_button_new_with_label("Next");
    panel->search = gtk_search_entry_new();
    g_object_set(panel->search, "placeholder-text", "Filter name, path or origin", NULL);
    gtk_widget_set_hexpand(panel->search, TRUE);
    gtk_box_append(GTK_BOX(actions), refresh_button);
    gtk_box_append(GTK_BOX(actions), panel->previous);
    gtk_box_append(GTK_BOX(actions), panel->next);
    gtk_box_append(GTK_BOX(actions), panel->search);
    panel->viewer = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(panel->viewer), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(panel->viewer), TRUE);
    GtkWidget *source_scroll = gtk_scrolled_window_new(), *rows_scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(source_scroll), panel->viewer);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(rows_scroll), panel->rows);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(rows_scroll), 160);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(source_scroll), 240);
    gtk_widget_set_vexpand(source_scroll, TRUE);
    gtk_box_append(GTK_BOX(panel->root), explanation);
    gtk_box_append(GTK_BOX(panel->root), actions);
    gtk_box_append(GTK_BOX(panel->root), panel->status);
    gtk_box_append(GTK_BOX(panel->root), rows_scroll);
    gtk_box_append(GTK_BOX(panel->root), panel->content_status);
    gtk_box_append(GTK_BOX(panel->root), source_scroll);
    (void)umi_gtk4_automation_tag_widget(panel->root, "debug.sources.panel");
    (void)umi_gtk4_automation_tag_widget(refresh_button, "debug.sources.refresh");
    (void)umi_gtk4_automation_tag_widget(panel->search, "debug.sources.search");
    (void)umi_gtk4_automation_tag_widget(panel->previous, "debug.sources.previous");
    (void)umi_gtk4_automation_tag_widget(panel->next, "debug.sources.next");
    (void)umi_gtk4_automation_tag_widget(panel->viewer, "debug.sources.text");
    (void)umi_gtk4_automation_tag_widget(panel->content_status, "debug.sources.content-status");
    g_object_set_data_full(G_OBJECT(panel->root), "umicom-source-panel", panel, SourcePanelFree);
    g_signal_connect_object(refresh_button, "clicked", G_CALLBACK(SourceRefresh), panel->root, 0);
    g_signal_connect_object(panel->search, "changed", G_CALLBACK(SourceSearch), panel->root, 0);
    g_signal_connect_object(panel->previous, "clicked", G_CALLBACK(SourcePrevious), panel->root, 0);
    g_signal_connect_object(panel->next, "clicked", G_CALLBACK(SourceNext), panel->root, 0);
    SourceRowsClear(panel);
    return panel->root;
}
