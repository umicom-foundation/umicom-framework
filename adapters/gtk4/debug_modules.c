/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/debug_modules.c
 * PURPOSE: Present bounded module pages without treating adapter paths as local files.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/debug_modules.h"
#include "umicom/ui/gtk4/automation.h"
#include <limits.h>
#include <string.h>
typedef struct ModulePanel
{
    GtkWidget *root, *rows, *status, *previous, *next;
    UmiDebugModulePage page;
    UmiGtk4DebugModulesQuery query;
    void *context;
    GDestroyNotify destroy;
    gboolean valid, pending;
} ModulePanel;
static void ModulePanelFree(gpointer data)
{
    ModulePanel *panel = data;
    if (panel->destroy != NULL)
        panel->destroy(panel->context);
    g_free(panel);
}
static void ModulePanelClear(ModulePanel *panel)
{
    GtkWidget *child;
    while ((child = gtk_widget_get_first_child(panel->rows)) != NULL)
        gtk_box_remove(GTK_BOX(panel->rows), child);
    panel->valid = FALSE;
    gtk_widget_set_sensitive(panel->previous, FALSE);
    gtk_widget_set_sensitive(panel->next, FALSE);
}
static void ModulePanelField(GtkWidget *box, const char *name, const char *value)
{
    char *text = g_strdup_printf("%s: %s", name, value[0] != '\0' ? value : "Not reported");
    GtkWidget *label = gtk_label_new(text);
    g_free(text);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_selectable(GTK_LABEL(label), TRUE);
    gtk_box_append(GTK_BOX(box), label);
}
static bool ModuleTextValid(const char *text, size_t capacity)
{
    return memchr(text, '\0', capacity) != NULL && g_utf8_validate(text, -1, NULL);
}
static bool ModulePageValid(const UmiDebugModulePage *page, uint32_t first)
{
    /* Host callbacks are an ownership boundary too: reject an incomplete identity
     * before retaining it for a later Next or Previous action. */
    if (page->session.generation == 0U || page->session.session_id[0] == '\0' ||
        !page->session.supported || page->first != first ||
        page->requested_count != UMI_DEBUG_MODULE_PAGE_LIMIT ||
        page->count > UMI_DEBUG_MODULE_PAGE_LIMIT ||
        !ModuleTextValid(page->session.session_id, sizeof page->session.session_id))
        return false;
    for (size_t i = 0U; i < page->count; ++i)
    {
        const UmiDebugModuleRecord *item = &page->items[i];
#define MODULE_VALID(field)                                                                        \
    if (!ModuleTextValid(item->field, sizeof item->field))                                         \
    return false
        MODULE_VALID(id);
        MODULE_VALID(name);
        MODULE_VALID(path);
        MODULE_VALID(version);
        MODULE_VALID(symbol_status);
        MODULE_VALID(symbol_path);
        MODULE_VALID(timestamp);
        MODULE_VALID(address_range);
#undef MODULE_VALID
    }
    return true;
}
static void ModulePanelRender(ModulePanel *panel, const UmiDebugModulePage *page)
{
    /* Replace rows only after the complete page has passed validation. Each widget
     * copies its text, so a host may release its response after this call. */
    ModulePanelClear(panel);
    panel->page = *page;
    panel->valid = TRUE;
    for (size_t i = 0U; i < page->count; ++i)
    {
        const UmiDebugModuleRecord *item = &page->items[i];
        char *title =
            item->numeric_id
                ? g_strdup_printf("%s — numeric ID %lld",
                                  item->name[0] ? item->name : "Unnamed module",
                                  (long long)item->number)
                : g_strdup_printf("%s — ID %s", item->name[0] ? item->name : "Unnamed module",
                                  item->id);
        GtkWidget *expander = gtk_expander_new(title);
        g_free(title);
        GtkWidget *details = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
        ModulePanelField(details, "Module path", item->path);
        ModulePanelField(details, "Version", item->version);
        ModulePanelField(details, "Symbols", item->symbol_status);
        ModulePanelField(details, "Symbol file", item->symbol_path);
        ModulePanelField(details, "Timestamp", item->timestamp);
        ModulePanelField(details, "Address range", item->address_range);
        ModulePanelField(details, "Optimized",
                         item->optimized_known ? (item->optimized ? "Yes" : "No") : "Not reported");
        ModulePanelField(details, "User code",
                         item->user_code_known ? (item->user_code ? "Yes" : "No") : "Not reported");
        gtk_expander_set_child(GTK_EXPANDER(expander), details);
        gtk_box_append(GTK_BOX(panel->rows), expander);
    }
    uint64_t end = (uint64_t)page->first + page->count;
    char *summary =
        page->total_known
            ? g_strdup_printf(
                  "%zu module%s on this page; first index %u. Adapter reports %llu total.",
                  page->count, page->count == 1U ? "" : "s", page->first,
                  (unsigned long long)page->total)
            : g_strdup_printf("%zu module%s on this page; first index %u. Total not reported%s.",
                              page->count, page->count == 1U ? "" : "s", page->first,
                              page->has_more ? "; another page may be available" : "");
    gtk_label_set_text(GTK_LABEL(panel->status), summary);
    g_free(summary);
    gtk_widget_set_sensitive(panel->previous, page->first != 0U);
    gtk_widget_set_sensitive(panel->next, page->has_more && page->count != 0U && end <= INT32_MAX);
}
static void ModulePanelQuery(ModulePanel *panel, const UmiDebugModuleSession *expected,
                             uint32_t first)
{
    if (panel->pending || !gtk_widget_get_mapped(panel->root))
        return;
    /* A host can close its window while servicing a synchronous query. Keep the
     * panel alive until the reply has been consumed, then release that hold. */
    GtkWidget *root = g_object_ref(panel->root);
    panel->pending = TRUE;
    UmiDebugModulePage *page = g_new0(UmiDebugModulePage, 1);
    UmiStatus status =
        panel->query(panel->context, expected, first, UMI_DEBUG_MODULE_PAGE_LIMIT, page);
    if (status == UMI_STATUS_OK && !ModulePageValid(page, first))
        status = UMI_STATUS_INVALID_ARGUMENT;
    if (status == UMI_STATUS_OK && expected != NULL &&
        (expected->generation != page->session.generation ||
         strcmp(expected->session_id, page->session.session_id) != 0))
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK)
        ModulePanelRender(panel, page);
    else
    {
        ModulePanelClear(panel);
        gtk_label_set_text(GTK_LABEL(panel->status),
                           status == UMI_STATUS_NOT_IMPLEMENTED
                               ? "This adapter did not advertise module browsing."
                           : status == UMI_STATUS_NOT_FOUND
                               ? "Start or attach a debugger, then choose Refresh modules."
                               : umi_status_text(status));
    }
    g_free(page);
    panel->pending = FALSE;
    g_object_unref(root);
}
static void ModulePanelRefresh(GtkButton *button, gpointer data)
{
    (void)button;
    ModulePanel *panel = g_object_get_data(G_OBJECT(data), "umicom-module-panel");
    ModulePanelQuery(panel, NULL, 0U);
}
static void ModulePanelPrevious(GtkButton *button, gpointer data)
{
    (void)button;
    ModulePanel *panel = g_object_get_data(G_OBJECT(data), "umicom-module-panel");
    if (!panel->valid || panel->page.first == 0U)
        return;
    UmiDebugModuleSession session = panel->page.session;
    uint32_t first = panel->page.first > UMI_DEBUG_MODULE_PAGE_LIMIT
                         ? panel->page.first - UMI_DEBUG_MODULE_PAGE_LIMIT
                         : 0U;
    ModulePanelQuery(panel, &session, first);
}
static void ModulePanelNext(GtkButton *button, gpointer data)
{
    (void)button;
    ModulePanel *panel = g_object_get_data(G_OBJECT(data), "umicom-module-panel");
    uint64_t first = (uint64_t)panel->page.first + panel->page.count;
    if (!panel->valid || !panel->page.has_more || panel->page.count == 0U || first > INT32_MAX)
        return;
    UmiDebugModuleSession session = panel->page.session;
    ModulePanelQuery(panel, &session, (uint32_t)first);
}
GtkWidget *UmiGtk4DebugModulesCreate(UmiGtk4DebugModulesQuery query, void *context,
                                     GDestroyNotify destroy)
{
    if (query == NULL)
        return NULL;
    ModulePanel *panel = g_new0(ModulePanel, 1);
    panel->query = query;
    panel->context = context;
    panel->destroy = destroy;
    panel->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    panel->rows = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    panel->status = gtk_label_new("Start or attach a debugger, then choose Refresh modules.");
    gtk_label_set_wrap(GTK_LABEL(panel->status), TRUE);
    gtk_label_set_xalign(GTK_LABEL(panel->status), 0.0F);
    GtkWidget *explanation =
        gtk_label_new("Paths and symbol status are supplied by the debugger. Refresh requests "
                      "current metadata; pages can change as modules load or unload.");
    gtk_label_set_wrap(GTK_LABEL(explanation), TRUE);
    gtk_label_set_xalign(GTK_LABEL(explanation), 0.0F);
    GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *refresh = gtk_button_new_with_label("Refresh modules");
    panel->previous = gtk_button_new_with_label("Previous");
    panel->next = gtk_button_new_with_label("Next");
    gtk_widget_set_sensitive(panel->previous, FALSE);
    gtk_widget_set_sensitive(panel->next, FALSE);
    gtk_box_append(GTK_BOX(actions), refresh);
    gtk_box_append(GTK_BOX(actions), panel->previous);
    gtk_box_append(GTK_BOX(actions), panel->next);
    gtk_box_append(GTK_BOX(panel->root), explanation);
    gtk_box_append(GTK_BOX(panel->root), actions);
    gtk_box_append(GTK_BOX(panel->root), panel->status);
    gtk_box_append(GTK_BOX(panel->root), panel->rows);
    (void)umi_gtk4_automation_tag_widget(panel->root, "debug.modules.panel");
    (void)umi_gtk4_automation_tag_widget(refresh, "debug.modules.refresh");
    (void)umi_gtk4_automation_tag_widget(panel->previous, "debug.modules.previous");
    (void)umi_gtk4_automation_tag_widget(panel->next, "debug.modules.next");
    (void)umi_gtk4_automation_tag_widget(panel->status, "debug.modules.status");
    g_object_set_data_full(G_OBJECT(panel->root), "umicom-module-panel", panel, ModulePanelFree);
    g_signal_connect_object(refresh, "clicked", G_CALLBACK(ModulePanelRefresh), panel->root, 0);
    g_signal_connect_object(panel->previous, "clicked", G_CALLBACK(ModulePanelPrevious),
                            panel->root, 0);
    g_signal_connect_object(panel->next, "clicked", G_CALLBACK(ModulePanelNext), panel->root, 0);
    return panel->root;
}
