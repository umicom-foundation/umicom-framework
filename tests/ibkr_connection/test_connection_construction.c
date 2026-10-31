/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_connection_construction.c
 *
 * PURPOSE:
 *   Verify broker window construction survives host callbacks without connecting or duplicating inspectors.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/access_gtk4.h"
#include "umicom/broker_connectivity/workspace_gtk4.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)

typedef struct Host {
    const char *scenario;
    GtkWindow *parent, *retained;
    UmiBrokerAccessPlan *input, expected;
    UmiBrokerWorkspaceGtk *workspace;
    unsigned observed;
    int failed;
    UmiStatus nested;
} Host;
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
static gboolean Registered(GtkWindow *window)
{
    GListModel *windows = gtk_window_get_toplevels();
    for (guint i = 0U; i < g_list_model_get_n_items(windows); ++i) {
        GtkWindow *item = g_list_model_get_item(windows, i);
        gboolean same = item == window;
        g_object_unref(item);
        if (same) return TRUE;
    }
    return FALSE;
}
static int Fields(GtkWindow *window, const UmiBrokerAccessPlan *plan)
{
    CHECK(window != NULL);
    GtkWidget *root = GTK_WIDGET(window);
    GtkWidget *mode = Find(root, "ibkr-mode"), *program = Find(root, "ibkr-program");
    GtkWidget *port = Find(root, "ibkr-port"), *client = Find(root, "ibkr-client");
    GtkWidget *ack = Find(root, "ibkr-live-ack"), *connect = Find(root, "ibkr-connect");
    CHECK(mode && program && port && client && ack && connect);
    CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(mode)) ==
        (plan->mode == UMI_BROKER_ACCESS_LIVE ? 1U : 0U));
    CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(program)) ==
        (plan->program == UMI_IBKR_GATEWAY ? 1U : 0U));
    CHECK(gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(port)) == (int)plan->port);
    CHECK(gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(client)) == plan->clientId);
    CHECK(!gtk_check_button_get_active(GTK_CHECK_BUTTON(ack)));
    return 0;
}
static int Idle(GtkWindow *window, const UmiBrokerAccessPlan *plan)
{
    UmiIbkrConnectionSnapshot *snapshot = g_new0(UmiIbkrConnectionSnapshot, 1);
    UmiStatus status = UmiIbkrGtkCopySnapshot(window, snapshot);
    gboolean correct = status == UMI_STATUS_OK && snapshot->state == UMI_IBKR_IDLE &&
        snapshot->readOnly && !snapshot->requestIssued && !snapshot->environmentAttested &&
        snapshot->requestedEnvironment == (plan->mode == UMI_BROKER_ACCESS_LIVE
            ? UMI_TRADING_LIVE : UMI_TRADING_PAPER);
    g_free(snapshot);
    CHECK(correct);
    return 0;
}
/* Observe the actual ownership transition before the constructor returns.
 * Real widgets are used, but no test opens a broker socket. */
static void Added(GtkApplication *application, GtkWindow *window, gpointer data)
{
    Host *host = data;
    if (window == host->parent) return;
    ++host->observed;
    if (host->observed != 1U || gtk_window_get_application(window) != application ||
        gtk_window_get_transient_for(window) != host->parent ||
        Fields(window, &host->expected) != 0) {
        host->failed = 1;
        return;
    }
    UmiIbkrConnectionSnapshot *snapshot = g_new(UmiIbkrConnectionSnapshot, 1);
    UmiIbkrConnectionSnapshot *before = g_new(UmiIbkrConnectionSnapshot, 1);
    memset(snapshot, 0x5a, sizeof(*snapshot)); memcpy(before, snapshot, sizeof(*before));
    if (UmiIbkrGtkCopySnapshot(window, snapshot) != UMI_STATUS_CANCELLED ||
        memcmp(snapshot, before, sizeof(*snapshot)) != 0 ||
        UmiIbkrGtkReadAccount(window, "SYNTHETIC") != UMI_STATUS_CANCELLED) host->failed = 1;
    g_free(before); g_free(snapshot);
    const char *which = host->scenario;
    if (strcmp(which, "callback-child") == 0 || strcmp(which, "after-child") == 0)
        gtk_window_destroy(window);
    else if (strcmp(which, "callback-retained") == 0) {
        host->retained = g_object_ref(window);
        gtk_window_destroy(window);
    } else if (strcmp(which, "callback-parent") == 0 || strcmp(which, "workspace-parent") == 0)
        gtk_window_destroy(host->parent);
    else if (strcmp(which, "callback-detach") == 0)
        gtk_application_remove_window(application, window);
    else if (strcmp(which, "callback-replace") == 0)
        gtk_window_set_child(window, gtk_label_new("Host replacement"));
    else if (strcmp(which, "callback-plan") == 0) {
        host->input->mode = UMI_BROKER_ACCESS_PAPER;
        host->input->port = 1U;
        host->input->clientId = 2;
    } else if (strcmp(which, "callback-command") == 0) {
        /* A direct signal bypasses widget sensitivity. The owner boundary
         * must still refuse execution until construction has finished. */
        g_signal_emit_by_name(Find(GTK_WIDGET(window), "ibkr-connect"), "clicked");
    } else if (strcmp(which, "workspace-reentrant") == 0)
        host->nested = UmiBrokerWorkspaceGtkOpenConnection(host->workspace);
    else if (strcmp(which, "workspace-destroy") == 0) {
        UmiBrokerWorkspaceGtkDestroy(host->workspace);
        host->workspace = NULL;
    }
}
static int Plain(const char *scenario)
{
    UmiBrokerAccessPlan plan;
    CHECK(UmiBrokerAccessPlanCreate(strcmp(scenario, "live") == 0
        ? UMI_BROKER_ACCESS_LIVE : UMI_BROKER_ACCESS_PAPER, &plan) == UMI_STATUS_OK);
    GtkWindow *parent = NULL, *window = NULL;
    if (strcmp(scenario, "unregistered-parent") == 0) {
        parent = GTK_WINDOW(gtk_window_new()); g_object_ref(parent);
        gtk_window_destroy(parent);
        CHECK(UmiIbkrGtkCreate(parent) == NULL);
        CHECK(UmiIbkrGtkCreateForEnvironment(parent, 0) == NULL);
        CHECK(UmiIbkrGtkCreateForPlan(parent, &plan) == NULL);
        g_object_unref(parent);
        return 0;
    }
    if (strcmp(scenario, "nonwindow-parent") == 0) {
        GtkWidget *label = gtk_label_new("Not a window"); g_object_ref_sink(label);
        CHECK(UmiIbkrGtkCreate((GtkWindow *)label) == NULL);
        CHECK(UmiIbkrGtkCreateForPlan((GtkWindow *)label, &plan) == NULL);
        g_object_unref(label);
        return 0;
    }
    if (strcmp(scenario, "parent-without-app") == 0) {
        parent = GTK_WINDOW(gtk_window_new()); g_object_ref(parent);
        window = UmiIbkrGtkCreateForPlan(parent, &plan);
        CHECK(window != NULL && gtk_window_get_transient_for(window) == parent);
        CHECK(gtk_window_get_application(window) == NULL);
    } else if (strcmp(scenario, "default") == 0) window = UmiIbkrGtkCreate(NULL);
    else if (strcmp(scenario, "custom") == 0) {
        CHECK(UmiBrokerAccessPlanSetProgram(&plan, UMI_IBKR_GATEWAY) == UMI_STATUS_OK);
        plan.port = 4011U; plan.clientId = 81;
        window = UmiIbkrGtkCreateForPlan(NULL, &plan);
    } else window = UmiIbkrGtkCreateForEnvironment(NULL, strcmp(scenario, "live") == 0 ? 1 : 0);
    CHECK(Fields(window, &plan) == 0 && Idle(window, &plan) == 0);
    gtk_window_destroy(window);
    if (parent != NULL) { gtk_window_destroy(parent); g_object_unref(parent); }
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *plain[] = {"default", "paper", "live", "custom", "unregistered-parent",
        "nonwindow-parent", "parent-without-app"};
    const char *callbacks[] = {"prepared", "callback-child", "after-child", "callback-parent",
        "callback-retained", "callback-detach", "callback-replace", "callback-plan",
        "callback-command", "workspace-reentrant", "workspace-destroy", "workspace-parent"};
    gboolean known = FALSE, isPlain = FALSE;
    for (size_t i = 0U; i < G_N_ELEMENTS(plain); ++i)
        if (strcmp(argv[1], plain[i]) == 0) known = isPlain = TRUE;
    for (size_t i = 0U; i < G_N_ELEMENTS(callbacks); ++i)
        if (strcmp(argv[1], callbacks[i]) == 0) known = TRUE;
    if (!known) return 2;
    if (!gtk_init_check()) return 77;
    if (isPlain) return Plain(argv[1]);
    GtkApplication *application = gtk_application_new(
        "org.umicom.tests.brokerconstruction", G_APPLICATION_NON_UNIQUE);
    CHECK(g_application_register(G_APPLICATION(application), NULL, NULL));
    GtkWindow *parent = GTK_WINDOW(gtk_window_new()); g_object_ref(parent);
    gtk_application_add_window(application, parent);
    UmiBrokerAccessPlan plan;
    CHECK(UmiBrokerAccessPlanCreate(UMI_BROKER_ACCESS_LIVE, &plan) == UMI_STATUS_OK);
    CHECK(UmiBrokerAccessPlanSetProgram(&plan, UMI_IBKR_GATEWAY) == UMI_STATUS_OK);
    plan.port = 4012U; plan.clientId = 82;
    Host host = {.scenario = argv[1], .parent = parent, .input = &plan,
        .expected = plan, .nested = UMI_STATUS_OK};
    gboolean workspace = strncmp(argv[1], "workspace-", 10U) == 0;
    if (workspace) {
        CHECK(UmiBrokerWorkspaceGtkCreate(parent, &plan, &host.workspace) == UMI_STATUS_OK);
        gtk_window_set_child(parent, UmiBrokerWorkspaceGtkWidget(host.workspace));
    }
    gulong signal = strcmp(argv[1], "after-child") == 0
        ? g_signal_connect_after(application, "window-added", G_CALLBACK(Added), &host)
        : g_signal_connect(application, "window-added", G_CALLBACK(Added), &host);
    if (workspace) {
        UmiStatus status = UmiBrokerWorkspaceGtkOpenConnection(host.workspace);
        CHECK(status == (strcmp(argv[1], "workspace-reentrant") == 0
            ? UMI_STATUS_OK : UMI_STATUS_CANCELLED));
        if (strcmp(argv[1], "workspace-reentrant") == 0) {
            CHECK(host.nested == UMI_STATUS_BUSY);
            CHECK(UmiBrokerWorkspaceGtkOpenConnection(host.workspace) == UMI_STATUS_OK);
            CHECK(host.observed == 1U);
        }
        if (host.workspace != NULL) UmiBrokerWorkspaceGtkDestroy(host.workspace);
    } else {
        GtkWindow *window = UmiIbkrGtkCreateForPlan(parent, &plan);
        gboolean succeeds = strcmp(argv[1], "prepared") == 0 ||
            strcmp(argv[1], "callback-plan") == 0 || strcmp(argv[1], "callback-command") == 0;
        if (succeeds) {
            CHECK(Fields(window, &host.expected) == 0 && Idle(window, &host.expected) == 0);
            gtk_window_destroy(window);
        } else CHECK(window == NULL);
    }
    CHECK(host.observed == 1U && host.failed == 0);
    if (host.retained != NULL) {
        CHECK(!Registered(host.retained));
        CHECK(UmiIbkrGtkReadAccount(host.retained, "SYNTHETIC") == UMI_STATUS_CANCELLED);
        g_object_unref(host.retained);
    }
    g_signal_handler_disconnect(application, signal);
    CHECK(g_list_length(gtk_application_get_windows(application)) ==
        (Registered(parent) ? 1U : 0U));
    if (Registered(parent)) gtk_window_destroy(parent);
    g_object_unref(parent); g_object_unref(application);
    return 0;
}
