/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_broker_welcome.c
 *
 * PURPOSE:
 *   Exercise real mode controls, callback reentry and retained-widget retirement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/welcome_gtk4.h"
#include <stdio.h>
#include <string.h>
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "Check failed at %d: %s\n", __LINE__, #condition); return 1; } } while (0)
typedef struct Probe {
    UmiBrokerWelcomeGtk *welcome;
    GtkWidget *reenter;
    unsigned calls, profileCalls;
    UmiBrokerAccessMode mode;
    UmiIbkrProgram program;
    uint16_t port;
    gboolean destroy;
    UmiStatus result;
} Probe;

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
static gboolean HasEntry(GtkWidget *root)
{
    if (GTK_IS_ENTRY(root) || GTK_IS_PASSWORD_ENTRY(root)) return TRUE;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL;
         child = gtk_widget_get_next_sibling(child))
        if (HasEntry(child)) return TRUE;
    return FALSE;
}
static UmiStatus Open(void *data, const UmiBrokerAccessPlan *plan)
{
    Probe *probe = data;
    ++probe->calls;
    probe->mode = plan->mode;
    probe->program = plan->program;
    probe->port = plan->port;
    if (probe->reenter != NULL) g_signal_emit_by_name(probe->reenter, "clicked");
    if (probe->destroy) {
        UmiBrokerWelcomeGtkDestroy(probe->welcome);
        probe->welcome = NULL;
    }
    return probe->result;
}
static UmiStatus Profiles(void *data)
{
    Probe *probe = data;
    ++probe->profileCalls;
    return UMI_STATUS_OK;
}
static void CloseOnSensitivity(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object; (void)property;
    Probe *probe = data;
    if (probe->welcome != NULL) {
        UmiBrokerWelcomeGtkDestroy(probe->welcome);
        probe->welcome = NULL;
    }
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (!gtk_init_check()) return 77;
    Probe probe = {0};
    UmiBrokerWelcomeGtkConfig config = {&probe, Open, Profiles};
    CHECK(UmiBrokerWelcomeGtkCreate(&config, &probe.welcome) == UMI_STATUS_OK);
    GtkWidget *root = UmiBrokerWelcomeGtkWidget(probe.welcome);
    GtkWidget *guest = Find(root, "broker-welcome-guest");
    GtkWidget *paper = Find(root, "broker-welcome-paper");
    GtkWidget *live = Find(root, "broker-welcome-live");
    GtkWidget *profiles = Find(root, "broker-welcome-profiles");
    CHECK(guest && paper && live && profiles);
    CHECK(probe.calls == 0U && !HasEntry(root));
    g_object_ref(root); g_object_ref(guest);
    if (strcmp(argv[1], "modes") == 0) {
        g_signal_emit_by_name(guest, "clicked");
        CHECK(probe.calls == 1U && probe.mode == UMI_BROKER_ACCESS_GUEST);
        g_signal_emit_by_name(paper, "clicked");
        CHECK(probe.calls == 2U && probe.mode == UMI_BROKER_ACCESS_PAPER);
        g_signal_emit_by_name(live, "clicked");
        CHECK(probe.calls == 3U && probe.mode == UMI_BROKER_ACCESS_LIVE);
        g_signal_emit_by_name(profiles, "clicked");
        CHECK(probe.profileCalls == 1U && probe.calls == 3U);
    } else if (strcmp(argv[1], "gateway") == 0) {
        GtkWidget *program = Find(root, "broker-welcome-program");
        CHECK(program != NULL);
        gtk_drop_down_set_selected(GTK_DROP_DOWN(program), 1U);
        g_signal_emit_by_name(paper, "clicked");
        CHECK(probe.program == UMI_IBKR_GATEWAY && probe.port == 4002U);
        g_signal_emit_by_name(live, "clicked");
        CHECK(probe.program == UMI_IBKR_GATEWAY && probe.port == 4001U);
        g_signal_emit_by_name(guest, "clicked");
        CHECK(probe.mode == UMI_BROKER_ACCESS_GUEST && probe.port == 0U);
    } else if (strcmp(argv[1], "retained") == 0) {
        UmiBrokerWelcomeGtkDestroy(probe.welcome); probe.welcome = NULL;
        g_signal_emit_by_name(guest, "clicked");
        CHECK(probe.calls == 0U);
    } else if (strcmp(argv[1], "reentrant") == 0) {
        probe.reenter = guest;
        g_signal_emit_by_name(guest, "clicked");
        CHECK(probe.calls == 1U);
    } else if (strcmp(argv[1], "close-callback") == 0) {
        probe.destroy = TRUE;
        g_signal_emit_by_name(paper, "clicked");
        CHECK(probe.calls == 1U && probe.welcome == NULL);
        g_signal_emit_by_name(guest, "clicked");
        CHECK(probe.calls == 1U);
    } else if (strcmp(argv[1], "close-notify") == 0) {
        g_signal_connect(guest, "notify::sensitive", G_CALLBACK(CloseOnSensitivity), &probe);
        g_signal_emit_by_name(guest, "clicked");
        CHECK(probe.calls == 0U && probe.welcome == NULL);
        g_signal_handlers_disconnect_by_data(guest, &probe);
    } else if (strcmp(argv[1], "failure") == 0) {
        probe.result = UMI_STATUS_UNAVAILABLE;
        g_signal_emit_by_name(paper, "clicked");
        CHECK(probe.calls == 1U && gtk_widget_get_sensitive(guest));
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(Find(root, "broker-welcome-status"))), "could not open"));
        probe.result = UMI_STATUS_OK;
        g_signal_emit_by_name(guest, "clicked");
        CHECK(probe.calls == 2U);

    } else if (strcmp(argv[1], "account-tabs") == 0) {
        GtkWidget *modes = Find(root, "broker-welcome-modes");
        GtkWidget *options = Find(root, "broker-welcome-connection-options");
        CHECK(GTK_IS_STACK(modes) && options != NULL);
        CHECK(strcmp(gtk_stack_get_visible_child_name(GTK_STACK(modes)), "paper") == 0);
        CHECK(gtk_widget_get_visible(options));
        gtk_stack_set_visible_child_name(GTK_STACK(modes), "demo");
        CHECK(!gtk_widget_get_visible(options) && probe.calls == 0U);
        gtk_stack_set_visible_child_name(GTK_STACK(modes), "live");
        CHECK(gtk_widget_get_visible(options) && probe.calls == 0U);
        CHECK(gtk_widget_get_ancestor(live, GTK_TYPE_STACK) == modes);
        CHECK(gtk_widget_get_ancestor(paper, GTK_TYPE_STACK) == modes);
        CHECK(gtk_widget_get_ancestor(guest, GTK_TYPE_STACK) == modes);
        CHECK(Find(root, "umicom.brand.logo") != NULL);
        CHECK(strstr(gtk_button_get_label(GTK_BUTTON(guest)), "Demo") != NULL);
    } else if (strcmp(argv[1], "retained-tabs") == 0) {
        GtkWidget *modes = Find(root, "broker-welcome-modes");
        CHECK(GTK_IS_STACK(modes));
        UmiBrokerWelcomeGtkDestroy(probe.welcome); probe.welcome = NULL;
        gtk_stack_set_visible_child_name(GTK_STACK(modes), "demo");
        gtk_stack_set_visible_child_name(GTK_STACK(modes), "live");
        CHECK(probe.calls == 0U);
    } else if (strcmp(argv[1], "optional-profiles") == 0) {
        UmiBrokerWelcomeGtk *other = NULL;
        config.openLocalProfiles = NULL;
        CHECK(UmiBrokerWelcomeGtkCreate(&config, &other) == UMI_STATUS_OK);
        CHECK(Find(UmiBrokerWelcomeGtkWidget(other), "broker-welcome-profiles") == NULL);
        UmiBrokerWelcomeGtkDestroy(other);
    } else return 2;
    UmiBrokerWelcomeGtkDestroy(probe.welcome);
    g_object_unref(guest); g_object_unref(root);
    return 0;
}
