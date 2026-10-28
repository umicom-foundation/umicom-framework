/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_profile_native.c
 *
 * PURPOSE:
 *   Check explicit native profile operations, Live consent and asynchronous teardown.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/access_gtk4.h"
#include "umicom/broker_connectivity/access_profiles.h"
#include "umicom/platform/filesystem.h"
#include <glib/gstdio.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)
static GtkWidget *Find(GtkWidget *root, const char *name)
{
    if (strcmp(gtk_widget_get_name(root), name) == 0) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL;
         child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = Find(child, name);
        if (found != NULL) return found;
    }
    return NULL;
}
static gboolean WaitEnabled(GtkWidget *button)
{
    gint64 deadline = g_get_monotonic_time() + 5000000;
    while (!gtk_widget_get_sensitive(button) && g_get_monotonic_time() < deadline) {
        (void)g_main_context_iteration(NULL, FALSE);
        g_usleep(1000U);
    }
    return gtk_widget_get_sensitive(button);
}
static void Finalized(gpointer data, GObject *object)
{
    (void)object;
    *(gboolean *)data = TRUE;
}
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    if (!gtk_init_check()) return 77;
    gchar *directory = g_build_filename(argv[2], "profile-XXXXXX", NULL);
    CHECK(g_mkdtemp(directory) != NULL);
    gchar *path = g_build_filename(directory, "profiles.txt", NULL);
    gchar *guard = g_strconcat(path, ".umi-guard", NULL);
    UmiBrokerAccessPlan plan;
    CHECK(UmiBrokerAccessPlanCreate(UMI_BROKER_ACCESS_LIVE, &plan) == UMI_STATUS_OK);
    plan.program = UMI_IBKR_GATEWAY; plan.port = 7777U; plan.clientId = 112;
    GtkWindow *window = UmiIbkrGtkCreateForPlan(NULL, &plan);
    CHECK(window != NULL);
    g_object_ref(window);
    GtkWidget *root = GTK_WIDGET(window);
    GtkWidget *save = Find(root, "ibkr-profile-save"), *load = Find(root, "ibkr-profile-load");
    GtkWidget *entry = Find(root, "ibkr-profile-path"), *notice = Find(root, "ibkr-profile-status");
    GtkWidget *port = Find(root, "ibkr-port"), *ack = Find(root, "ibkr-live-ack");
    GtkWidget *connect = Find(root, "ibkr-connect"), *disconnect = Find(root, "ibkr-disconnect");
    CHECK(save && load && entry && notice && port && ack && connect && disconnect);
    gtk_editable_set_text(GTK_EDITABLE(entry), path);
    CHECK(!g_file_test(path, G_FILE_TEST_EXISTS));
    const char *name = argv[1];
    if (strcmp(name, "save-load") == 0) {
        gtk_check_button_set_active(GTK_CHECK_BUTTON(ack), TRUE);
        g_signal_emit_by_name(save, "clicked");
        CHECK(!gtk_widget_get_sensitive(connect));
        /* Synthetic clicks must respect the same busy gate as visible controls. */
        g_signal_emit_by_name(connect, "clicked");
        CHECK(!gtk_widget_get_sensitive(disconnect));
        CHECK(WaitEnabled(save));
        UmiBrokerAccessProfiles profiles;
        CHECK(UmiBrokerAccessProfilesLoad(path, &profiles) == UMI_STATUS_OK);
        CHECK(profiles.live.port == 7777U && profiles.live.clientId == 112);
        CHECK(profiles.paper.port == 7497U);
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(port), 9999);
        g_signal_emit_by_name(load, "clicked");
        CHECK(WaitEnabled(load));
        CHECK(gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(port)) == 7777);
        CHECK(!gtk_check_button_get_active(GTK_CHECK_BUTTON(ack)));
        CHECK(!gtk_widget_get_sensitive(disconnect));
    } else if (strcmp(name, "missing") == 0) {
        g_signal_emit_by_name(load, "clicked");
        CHECK(WaitEnabled(load));
        CHECK(gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(port)) == 7777);
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(notice)), "No saved settings") != NULL);
        CHECK(!g_file_test(path, G_FILE_TEST_EXISTS));
    } else if (strcmp(name, "damaged") == 0) {
        const char *broken = "future format; preserve this text";
        CHECK(g_file_set_contents(path, broken, -1, NULL));
        g_signal_emit_by_name(save, "clicked");
        CHECK(WaitEnabled(save));
        gchar *bytes = NULL;
        CHECK(g_file_get_contents(path, &bytes, NULL, NULL));
        CHECK(strcmp(bytes, broken) == 0); g_free(bytes);
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(notice)), "not been replaced") != NULL);
    } else if (strcmp(name, "endpoint-ack") == 0) {
        gtk_check_button_set_active(GTK_CHECK_BUTTON(ack), TRUE);
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(port), 9999);
        CHECK(!gtk_check_button_get_active(GTK_CHECK_BUTTON(ack)));
        gtk_check_button_set_active(GTK_CHECK_BUTTON(ack), TRUE);
        GtkWidget *client = Find(root, "ibkr-client");
        CHECK(client != NULL);
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(client), 456);
        CHECK(!gtk_check_button_get_active(GTK_CHECK_BUTTON(ack)));
    } else if (strcmp(name, "stale-fields") == 0) {
        CHECK(UmiBrokerAccessProfilesStore(path, &plan) == UMI_STATUS_OK);
        g_signal_emit_by_name(load, "clicked");
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(port), 9999);
        CHECK(WaitEnabled(load));
        CHECK(gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(port)) == 9999);
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(notice)), "fields changed") != NULL);
    } else if (strcmp(name, "retired") == 0) {
        gboolean finalized = FALSE;
        g_object_weak_ref(G_OBJECT(window), Finalized, &finalized);
        g_object_ref(save);
        g_signal_emit_by_name(save, "clicked");
        gtk_window_destroy(window);
        g_signal_emit_by_name(save, "clicked");
        g_object_unref(window); window = NULL;
        g_object_unref(save);
        gint64 deadline = g_get_monotonic_time() + 5000000;
        while (!finalized && g_get_monotonic_time() < deadline) {
            (void)g_main_context_iteration(NULL, FALSE); g_usleep(1000U);
        }
        CHECK(finalized);
    } else return 2;
    if (window != NULL) { gtk_window_destroy(window); g_object_unref(window); }
    /* Each test owns its unique directory. Workers have completed before it is removed. */
    if (g_file_test(path, G_FILE_TEST_EXISTS)) CHECK(g_remove(path) == 0);
    if (g_file_test(guard, G_FILE_TEST_EXISTS)) CHECK(g_remove(guard) == 0);
    CHECK(g_rmdir(directory) == 0);
    g_free(guard); g_free(path); g_free(directory);
    return 0;
}
