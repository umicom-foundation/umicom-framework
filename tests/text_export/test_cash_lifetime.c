/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/text_export/test_cash_lifetime.c
 * PURPOSE: Retire cash-report exports at native removal, before finalisation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/cash_plan.h"
#include "umicom/ui/gtk4/text_export.h"
#include "umicom/ui/gtk4/automation.h"
#include <stdio.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "Line %d: %s\n", __LINE__, #c); result = 1; goto cleanup; } } while (0)
int main(void)
{
    if (!gtk_init_check()) return 77;
    int result = 0;
    GtkWindow *parent = GTK_WINDOW(gtk_window_new()); g_object_ref_sink(parent);
    GtkWidget *launcher = UmiGtk4CashPlanLauncherCreate();
    gtk_window_set_child(parent, launcher);
    GtkWindow *cash = NULL;
    GtkWidget *root = NULL, *export = NULL, *save = NULL;
    g_signal_emit_by_name(launcher, "clicked");
    GListModel *windows = gtk_window_get_toplevels();
    for (guint i = 0; i < g_list_model_get_n_items(windows); ++i) {
        GtkWindow *candidate = g_list_model_get_item(windows, i);
        const char *title = gtk_window_get_title(candidate);
        if (candidate != parent && title != NULL && strcmp(title, "Cash planning") == 0) {
            cash = candidate; break;
        }
        g_object_unref(candidate);
    }
    CHECK(cash != NULL);
    root = g_object_ref(gtk_window_get_child(cash));
    export = umi_gtk4_automation_find_tagged_widget(root, "cash.plan.text-export");
    CHECK(export != NULL && gtk_widget_get_sensitive(export));
    save = umi_gtk4_automation_find_tagged_widget(export, "text.export.save");
    CHECK(save != NULL);
    /* Keep both the root and window referenced. Finalisation cannot be what
     * retires this control; the toplevel removal observer must do it now. */
    gtk_window_destroy(cash);
    CHECK(!gtk_widget_get_sensitive(export));
    g_signal_emit_by_name(save, "clicked");
    CHECK(!UmiTextExportGtk4Busy(export));
cleanup:
    if (cash != NULL) { gtk_window_destroy(cash); g_object_unref(cash); }
    g_clear_object(&root);
    gtk_window_destroy(parent); g_object_unref(parent);
    return result;
}
