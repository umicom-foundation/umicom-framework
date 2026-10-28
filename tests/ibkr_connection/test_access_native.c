/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_access_native.c
 *
 * PURPOSE:
 *   Verify broker launchers copy settings without connecting or inheriting Live consent.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/access_gtk4.h"
#include <stdio.h>
#include <string.h>
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "Check failed at %d: %s\n", __LINE__, #condition); return 1; } } while (0)

static GtkWidget *Find(GtkWidget *root, const char *name)
{
    if (strcmp(gtk_widget_get_name(root), name) == 0) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL;
         child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *result = Find(child, name);
        if (result != NULL) return result;
    }
    return NULL;
}
static GtkWindow *ChildOf(GtkWindow *parent)
{
    GListModel *windows = gtk_window_get_toplevels();
    for (guint i = 0; i < g_list_model_get_n_items(windows); ++i) {
        GtkWindow *window = g_list_model_get_item(windows, i);
        if (gtk_window_get_transient_for(window) == parent) return window;
        g_object_unref(window);
    }
    return NULL;
}
static int CheckPlan(GtkWindow *window, const UmiBrokerAccessPlan *plan)
{
    CHECK(window != NULL);
    GtkWidget *root = GTK_WIDGET(window);
    GtkWidget *mode = Find(root, "ibkr-mode"), *program = Find(root, "ibkr-program");
    GtkWidget *port = Find(root, "ibkr-port"), *client = Find(root, "ibkr-client");
    GtkWidget *ack = Find(root, "ibkr-live-ack"), *disconnect = Find(root, "ibkr-disconnect");
    CHECK(mode && program && port && client && ack && disconnect);
    CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(mode)) ==
        (plan->mode == UMI_BROKER_ACCESS_LIVE ? 1U : 0U));
    CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(program)) ==
        (plan->program == UMI_IBKR_GATEWAY ? 1U : 0U));
    CHECK(gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(port)) == (int)plan->port);
    CHECK(gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(client)) == plan->clientId);
    CHECK(!gtk_check_button_get_active(GTK_CHECK_BUTTON(ack)));
    CHECK(!gtk_widget_get_sensitive(disconnect));
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (!gtk_init_check()) return 77;
    UmiBrokerAccessPlan plan;
    CHECK(UmiBrokerAccessPlanCreate(UMI_BROKER_ACCESS_LIVE, &plan) == UMI_STATUS_OK);
    GtkWindow *window = NULL;
    if (strcmp(argv[1], "defaults") == 0) {
        for (unsigned i = 0; i < 4U; ++i) {
            CHECK(UmiBrokerAccessPlanCreate(i < 2U ? UMI_BROKER_ACCESS_PAPER
                : UMI_BROKER_ACCESS_LIVE, &plan) == UMI_STATUS_OK);
            CHECK(UmiBrokerAccessPlanSetProgram(&plan, i % 2U == 0U ? UMI_IBKR_TWS
                : UMI_IBKR_GATEWAY) == UMI_STATUS_OK);
            window = UmiIbkrGtkCreateForPlan(NULL, &plan);
            CHECK(CheckPlan(window, &plan) == 0);
            gtk_window_destroy(window);
        }
    } else if (strcmp(argv[1], "custom") == 0) {
        CHECK(UmiBrokerAccessPlanSetProgram(&plan, UMI_IBKR_GATEWAY) == UMI_STATUS_OK);
        plan.port = 4567U; plan.clientId = 1234;
        window = UmiIbkrGtkCreateForPlan(NULL, &plan);
        CHECK(CheckPlan(window, &plan) == 0);
        gtk_window_destroy(window);
    } else if (strcmp(argv[1], "invalid") == 0) {
        CHECK(UmiIbkrGtkCreateForPlan(NULL, NULL) == NULL);
        CHECK(UmiBrokerAccessPlanCreate(UMI_BROKER_ACCESS_GUEST, &plan) == UMI_STATUS_OK);
        CHECK(UmiIbkrGtkCreateForPlan(NULL, &plan) == NULL);
        CHECK(UmiIbkrGtkLauncherCreateForPlan(NULL, &plan) == NULL);
        plan.mode = (UmiBrokerAccessMode)99;
        CHECK(UmiIbkrGtkCreateForPlan(NULL, &plan) == NULL);
    } else if (strcmp(argv[1], "consent-reopen") == 0) {
        window = UmiIbkrGtkCreateForPlan(NULL, &plan);
        CHECK(CheckPlan(window, &plan) == 0);
        gtk_check_button_set_active(GTK_CHECK_BUTTON(Find(GTK_WIDGET(window), "ibkr-live-ack")), TRUE);
        gtk_window_destroy(window);
        window = UmiIbkrGtkCreateForPlan(NULL, &plan);
        CHECK(CheckPlan(window, &plan) == 0);
        gtk_window_destroy(window);
    } else if (strcmp(argv[1], "launcher-copy") == 0 ||
               strcmp(argv[1], "parent-retired") == 0) {
        GtkWindow *parent = GTK_WINDOW(gtk_window_new());
        g_object_ref(parent);
        GtkWidget *launcher = UmiIbkrGtkLauncherCreateForPlan(parent, &plan);
        CHECK(launcher != NULL);
        g_object_ref_sink(launcher);
        if (strcmp(argv[1], "launcher-copy") == 0) {
            UmiBrokerAccessPlan expected = plan;
            plan.mode = UMI_BROKER_ACCESS_PAPER; plan.port = 1U;
            g_signal_emit_by_name(launcher, "clicked");
            window = ChildOf(parent);
            CHECK(CheckPlan(window, &expected) == 0);
            gtk_window_destroy(window); g_object_unref(window);
            gtk_window_destroy(parent);
        } else {
            gtk_window_destroy(parent);
            guint before = g_list_model_get_n_items(gtk_window_get_toplevels());
            g_signal_emit_by_name(launcher, "clicked");
            CHECK(g_list_model_get_n_items(gtk_window_get_toplevels()) == before);
        }
        g_object_unref(launcher); g_object_unref(parent);
    } else return 2;
    return 0;
}
