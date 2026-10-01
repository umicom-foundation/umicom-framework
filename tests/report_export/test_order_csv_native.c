/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/report_export/test_order_csv_native.c
 * PURPOSE: Exercise the actual order CSV action, applied filters and retained controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../trading_execution/order_review_fixture.h"
#include "umicom/trading_ui/gtk4/trading_panels.h"
#define CHECK REVIEW_CHECK
static GtkWidget *Find(GtkWidget *root, const char *id)
{
    const char *tag = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (tag != NULL && strcmp(tag, id) == 0) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL; child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = Find(child, id); if (found != NULL) return found;
    }
    return NULL;
}
static char *ClipboardText(GdkClipboard *clipboard)
{
    GdkContentProvider *provider = gdk_clipboard_get_content(clipboard);
    CHECK(provider != NULL);
    GValue value = G_VALUE_INIT; g_value_init(&value, G_TYPE_STRING);
    CHECK(gdk_content_provider_get_value(provider, &value, NULL));
    char *text = g_value_dup_string(&value); g_value_unset(&value); CHECK(text != NULL); return text;
}
int main(void)
{
    if (!gtk_init_check()) return 77;
    ReviewFixture f; ReviewFixtureInit(&f);
    UmiGtk4TradingPanelContext context = {f.workspace, &f.controller, 0};
    UmiUiWorkspaceWindow spec = {0}; strcpy(spec.tool_id, "blotter");
    GtkWidget *panel = umi_gtk4_trading_panel_create(&spec, &context); CHECK(panel != NULL);
    GtkWidget *window = gtk_window_new(); g_object_ref_sink(window);
    gtk_window_set_child(GTK_WINDOW(window), panel);
    GtkWidget *copy = Find(panel, "trading.orders.copy-csv"); CHECK(GTK_IS_BUTTON(copy)); g_object_ref(copy);
    GtkWidget *search = Find(panel, "trading.orders.search"); CHECK(GTK_IS_EDITABLE(search));
    /* An unsubmitted edit must not silently become the export filter. */
    gtk_editable_set_text(GTK_EDITABLE(search), "missing-order");
    UmiTradingWorkspaceSnapshot before, after;
    CHECK(umi_trading_workspace_snapshot(f.workspace, &before) == UMI_STATUS_OK);
    GdkClipboard *clipboard = g_object_ref(gtk_widget_get_clipboard(copy));
    g_signal_emit_by_name(copy, "clicked");
    char *text = ClipboardText(clipboard);
    CHECK(strstr(text, f.first) != NULL && strstr(text, f.second) != NULL);
    CHECK(strstr(text, "missing-order") == NULL); g_free(text);
    CHECK(umi_trading_workspace_snapshot(f.workspace, &after) == UMI_STATUS_OK && before.revision == after.revision);
    UmiTradingOrderQuery query = {UMI_TRADING_WORKSPACE_ORDERS_ALL, "nq"};
    CHECK(UmiTradingWorkspaceSetOrderQuery(f.workspace, &query) == UMI_STATUS_OK);
    g_signal_emit_by_name(copy, "clicked"); text = ClipboardText(clipboard);
    CHECK(strstr(text, f.first) != NULL && strstr(text, f.second) == NULL); g_free(text);
    /* A retained child must remain inert after its owner and workspace go. */
    gtk_window_destroy(GTK_WINDOW(window)); g_object_unref(window);
    umi_trading_workspace_destroy(f.workspace); context.workspace = NULL; context.controller = NULL;
    gdk_clipboard_set_text(clipboard, "retained-control-sentinel");
    g_signal_emit_by_name(copy, "clicked"); text = ClipboardText(clipboard);
    CHECK(strcmp(text, "retained-control-sentinel") == 0);
    g_free(text); g_object_unref(copy); g_object_unref(clipboard); return 0;
}
