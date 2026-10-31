/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_gateway_gtk4.c
 *
 * PURPOSE:
 *   Verify Gateway discovery and native ownership without launching a broker.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/gateway_gtk4.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) { g_printerr("Check failed at %d: %s\n", __LINE__, #x); return 1; } } while (0)
static GtkWidget *Find(GtkWidget *root, const char *name)
{
    if (!strcmp(gtk_widget_get_name(root), name)) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL;
         child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = Find(child, name); if (found != NULL) return found;
    }
    return NULL;
}
static gboolean WaitReady(GtkWidget *refresh)
{
    gint64 end = g_get_monotonic_time() + 3000000;
    while (!gtk_widget_get_sensitive(refresh) && g_get_monotonic_time() < end) {
        while (g_main_context_iteration(NULL, FALSE)) {}
        g_usleep(1000);
    }
    return gtk_widget_get_sensitive(refresh);
}
static void Retire(GObject *object, GParamSpec *spec, gpointer data)
{
    (void)object; (void)spec;
    UmiIbkrGatewayGtk **owner = data;
    if (*owner != NULL) { UmiIbkrGatewayGtk *old = *owner; *owner = NULL; UmiIbkrGatewayGtkDestroy(old); }
}
static int Run(const char *mode)
{
    const char *roots[] = {NULL};
    UmiIbkrGatewayGtk *owner = UmiIbkrGatewayGtkCreate(roots);
    GtkWidget *root = g_object_ref(UmiIbkrGatewayGtkWidget(owner));
    GtkWidget *refresh = Find(root, "ibkr-gateway-refresh");
    GtkWidget *open = Find(root, "ibkr-gateway-open");
    GtkWidget *status = Find(root, "ibkr-gateway-status");
    GtkWidget *custom = Find(root, "ibkr-gateway-custom-path");
    CHECK(refresh != NULL && open != NULL && status != NULL && custom != NULL);
    if (!strcmp(mode, "pending-close")) {
        UmiIbkrGatewayGtkDestroy(owner); owner = NULL;
        for (unsigned i = 0U; i < 100U; ++i) {
            while (g_main_context_iteration(NULL, FALSE)) {} g_usleep(1000);
        }
        CHECK(!gtk_widget_get_sensitive(root));
    } else {
        CHECK(WaitReady(refresh));
        CHECK(!gtk_widget_get_sensitive(open));
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(status)), "No Gateway found") != NULL);
        if (!strcmp(mode, "custom-invalid")) {
            gtk_editable_set_text(GTK_EDITABLE(custom), "relative/ibgateway.exe");
            CHECK(!gtk_widget_get_sensitive(open));
            g_signal_emit_by_name(refresh, "clicked");
            CHECK(WaitReady(refresh));
            CHECK(strstr(gtk_label_get_text(GTK_LABEL(status)), "could not be checked") != NULL);
        } else if (!strcmp(mode, "notify-close")) {
            g_signal_connect(refresh, "notify::sensitive", G_CALLBACK(Retire), &owner);
            g_signal_emit_by_name(refresh, "clicked");
            CHECK(owner == NULL);
        } else if (!strcmp(mode, "retained")) {
            UmiIbkrGatewayGtkDestroy(owner); owner = NULL;
            gchar *before = g_strdup(gtk_label_get_text(GTK_LABEL(status)));
            g_signal_emit_by_name(open, "clicked");
            g_signal_emit_by_name(refresh, "clicked");
            gtk_editable_set_text(GTK_EDITABLE(custom), "retired");
            CHECK(!strcmp(before, gtk_label_get_text(GTK_LABEL(status)))); g_free(before);
        }
    }
    if (owner != NULL) UmiIbkrGatewayGtkDestroy(owner);
    g_object_unref(root); return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (!gtk_init_check()) return 77;
    return Run(argv[1]);
}
