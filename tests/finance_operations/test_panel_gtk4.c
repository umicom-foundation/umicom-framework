/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_operations/test_panel_gtk4.c
 *
 * PURPOSE:
 *   Exercise the real command form and ensure retained widgets cannot call a released financial service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/finance_operations.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "GTK financial test failed: %s\n", #x); return 1; } } while (0)
static GtkWidget *Find(GtkWidget *root, const char *name)
{
    if (strcmp(gtk_widget_get_name(root), name) == 0) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL;
         child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *match = Find(child, name);
        if (match != NULL) return match;
    }
    return NULL;
}
int main(void)
{
    UmiDataServer *server = NULL;
    UmiFinanceOperations *operations = NULL;
    UmiFinanceOperationsGtkPanel *panel = NULL;
    UmiFinanceOperationCounts snapshot;
    GtkWidget *root, *apply, *record, *name, *scale;
    if (!gtk_init_check()) { puts("SKIP: no GTK display is available."); return 77; }
    CHECK(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    CHECK(UmiFinanceOperationsCreate(server, &operations) == UMI_STATUS_OK);
    CHECK(UmiFinanceOperationsGtkPanelCreate(operations, "Memory-only UI test", &panel) == UMI_STATUS_OK);
    root = UmiFinanceOperationsGtkPanelWidget(panel);
    CHECK(root != NULL); g_object_ref(root);
    apply = Find(root, "finance-apply"); record = Find(root, "finance-record");
    name = Find(root, "finance-name"); scale = Find(root, "finance-scale");
    CHECK(apply != NULL && record != NULL && name != NULL && scale != NULL);
    CHECK(UmiFinanceOperationsCounts(operations, &snapshot) == UMI_STATUS_OK && snapshot.revision == 0U);
    gtk_editable_set_text(GTK_EDITABLE(record), "control");
    gtk_editable_set_text(GTK_EDITABLE(name), "Practice control account");
    gtk_editable_set_text(GTK_EDITABLE(scale), "invalid");
    g_signal_emit_by_name(apply, "clicked");
    CHECK(UmiFinanceOperationsCounts(operations, &snapshot) == UMI_STATUS_OK && snapshot.revision == 0U);
    gtk_editable_set_text(GTK_EDITABLE(scale), "2");
    g_signal_emit_by_name(apply, "clicked");
    CHECK(UmiFinanceOperationsCounts(operations, &snapshot) == UMI_STATUS_OK && snapshot.revision == 1U);
    g_signal_emit_by_name(apply, "clicked");
    CHECK(UmiFinanceOperationsCounts(operations, &snapshot) == UMI_STATUS_OK && snapshot.revision == 1U);
    /* The embedding window may retain the root after its controller is released.
     * Such a widget must be inert, even before the final GObject reference goes. */
    UmiFinanceOperationsGtkPanelDestroy(panel); panel = NULL;
    gtk_editable_set_text(GTK_EDITABLE(record), "another");
    g_signal_emit_by_name(apply, "clicked");
    CHECK(UmiFinanceOperationsCounts(operations, &snapshot) == UMI_STATUS_OK && snapshot.revision == 1U);
    UmiFinanceOperationsDestroy(operations); umi_data_server_destroy(server);
    g_signal_emit_by_name(apply, "clicked");
    g_object_unref(root);
    puts("Financial form validation, duplicate safety and retained-widget lifetime passed.");
    return 0;
}
