/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/broker_welcome.c
 *
 * PURPOSE:
 *   Offer explicit trading modes without collecting broker login secrets.
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
#include "brand_widget.h"
#include "startup_brand_panel.h"
#include "umicom/broker_connectivity/gateway_gtk4.h"

struct UmiBrokerWelcomeGtk {
    UmiIbkrGatewayGtk *gateway;
    unsigned references;
    gboolean closed, busy;
    UmiBrokerWelcomeGtkConfig config;
    GtkWidget *root, *buttons[4], *status, *program;
    GtkWidget *modes, *connectionOptions;
    GtkWidget *stage, *begin[2], *back, *authenticationText;
};

static void Release(UmiBrokerWelcomeGtk *welcome)
{
    if (--welcome->references != 0U) return;
    g_object_unref(welcome->root);
    g_free(welcome);
}

static GtkWidget *Note(const char *text)
{
    GtkWidget *label = gtk_label_new(text);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    return label;
}

static void Choose(GtkButton *button, gpointer data)
{
    UmiBrokerWelcomeGtk *welcome = data;
    if (welcome->closed || welcome->busy) return;
    ++welcome->references;
    welcome->busy = TRUE;
    guint program = gtk_drop_down_get_selected(GTK_DROP_DOWN(welcome->program));
    gtk_widget_set_sensitive(welcome->program, FALSE);
    unsigned choice = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(button), "broker-choice"));
    for (size_t i = 0; i < G_N_ELEMENTS(welcome->buttons) && !welcome->closed; ++i)
        if (welcome->buttons[i] != NULL) gtk_widget_set_sensitive(welcome->buttons[i], FALSE);
    if (welcome->closed) { Release(welcome); return; }
    UmiStatus status;
    if (choice == 3U) {
        status = welcome->config.openLocalProfiles != NULL
            ? welcome->config.openLocalProfiles(welcome->config.userData)
            : UMI_STATUS_UNAVAILABLE;
    } else {
        UmiBrokerAccessPlan plan;
        status = UmiBrokerAccessPlanCreate((UmiBrokerAccessMode)choice, &plan);
        if (status == UMI_STATUS_OK && choice != 0U)
            status = UmiBrokerAccessPlanSetProgram(&plan, (UmiIbkrProgram)program);
        if (status == UMI_STATUS_OK)
            status = welcome->config.openMode(welcome->config.userData, &plan);
    }
    if (status != UMI_STATUS_OK)
        (void)fprintf(stderr, "Broker workspace startup failed: %s\n", umi_status_text(status));
    /* Product callbacks may replace the complete start screen. Holding this
     * reference until return protects that transition without extending authority. */
    if (!welcome->closed) {
        gtk_label_set_text(GTK_LABEL(welcome->status), status == UMI_STATUS_OK
            ? "The selected workspace has been requested."
            : "The workspace could not open. Your selection is still available; check the application log.");
        if (!welcome->closed) gtk_widget_set_sensitive(welcome->program, TRUE);
        for (size_t i = 0; i < G_N_ELEMENTS(welcome->buttons) && !welcome->closed; ++i)
            if (welcome->buttons[i] != NULL) gtk_widget_set_sensitive(welcome->buttons[i], TRUE);
        if (!welcome->closed) welcome->busy = FALSE;
    }
    Release(welcome);
}


/* Selection changes only the relevant connection controls. Retained widgets
 * cannot call this owner after it has been destroyed. */
static void ModeChanged(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object; (void)property;
    UmiBrokerWelcomeGtk *welcome = data;
    if (welcome->closed) return;
    ++welcome->references;
    const char *name = gtk_stack_get_visible_child_name(GTK_STACK(welcome->modes));
    gtk_widget_set_visible(welcome->connectionOptions, g_strcmp0(name, "demo") != 0);
    Release(welcome);
}


/* This is a local guidance transition, not an authentication request. No
 * username, password or second-factor value is collected by this controller. */
static void BeginAuthentication(GtkButton *button, gpointer data)
{
    UmiBrokerWelcomeGtk *welcome = data;
    if (welcome->closed || welcome->busy) return;
    ++welcome->references;
    unsigned mode = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(button), "broker-choice"));
    if (mode != 1U && mode != 2U) { Release(welcome); return; }
    /* Widget notifications may reenter actions while the stage is changing. */
    welcome->busy = TRUE;
    const char *program = gtk_drop_down_get_selected(GTK_DROP_DOWN(welcome->program)) == 1U
        ? "IB Gateway" : "TWS";
    char *guide = g_strdup_printf("1. Open %s and choose %s.\n\n"
        "2. Sign in and complete the broker's second-factor request.\n\n"
        "3. Enable local socket API access, then continue to Trader and choose Connect.",
        program, mode == 1U ? "Paper Trading" : "Live Trading");
    gtk_label_set_text(GTK_LABEL(welcome->authenticationText), guide);
    g_free(guide);
    if (!welcome->closed)
        gtk_widget_set_visible(UmiIbkrGatewayGtkWidget(welcome->gateway),
            gtk_drop_down_get_selected(GTK_DROP_DOWN(welcome->program)) == 1U);
    if (!welcome->closed) gtk_widget_set_visible(welcome->buttons[1], mode == 1U);
    if (!welcome->closed) gtk_widget_set_visible(welcome->buttons[2], mode == 2U);
    if (!welcome->closed)
        gtk_stack_set_visible_child_name(GTK_STACK(welcome->stage), "authentication");
    if (!welcome->closed) gtk_widget_grab_focus(welcome->buttons[mode]);
    if (!welcome->closed) welcome->busy = FALSE;
    Release(welcome);
}

static void BackToAccounts(GtkButton *button, gpointer data)
{
    (void)button;
    UmiBrokerWelcomeGtk *welcome = data;
    if (welcome->closed || welcome->busy) return;
    ++welcome->references;
    welcome->busy = TRUE;
    gtk_stack_set_visible_child_name(GTK_STACK(welcome->stage), "accounts");
    if (!welcome->closed) {
        const char *mode = gtk_stack_get_visible_child_name(GTK_STACK(welcome->modes));
        GtkWidget *focus = g_strcmp0(mode, "demo") == 0 ? welcome->buttons[0]
            : welcome->begin[g_strcmp0(mode, "live") == 0 ? 1U : 0U];
        gtk_widget_grab_focus(focus);
    }
    if (!welcome->closed) welcome->busy = FALSE;
    Release(welcome);
}

#include "broker_welcome_layout.inc"

UmiStatus UmiBrokerWelcomeGtkCreate(const UmiBrokerWelcomeGtkConfig *config,
    UmiBrokerWelcomeGtk **outWelcome)
{
    if (outWelcome == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outWelcome = NULL;
    if (config == NULL || config->openMode == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiBrokerWelcomeGtk *welcome = g_try_new0(UmiBrokerWelcomeGtk, 1);
    if (welcome == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    welcome->references = 1U;
    welcome->config = *config;
    welcome->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 14);
    g_object_ref_sink(welcome->root);
    gtk_widget_set_name(welcome->root, "broker-welcome");
    /* The stacked account view is retained for engineering comparison. The
     * replacement separates brand and account content with an adjustable divider
     * and keeps authentication guidance distinct from connection authority. */
#if 0
    gtk_widget_set_margin_start(welcome->root, 24);
    gtk_widget_set_margin_end(welcome->root, 24);
    gtk_widget_set_margin_top(welcome->root, 24);
    gtk_widget_set_margin_bottom(welcome->root, 24);
    /* The former stacked mode list is retained for review. The tabbed account
     * surface below keeps authentication guidance beside the selected mode. */
#if 0
    GtkWidget *title = Note("Choose Guest, Paper or Live");
    gtk_widget_add_css_class(title, "title-1");
    gtk_box_append(GTK_BOX(welcome->root), title);
    gtk_box_append(GTK_BOX(welcome->root), Note(
        "Broker passwords and authentication codes belong in TWS or IB Gateway. "
        "This screen does not collect or upload them."));
    gtk_box_append(GTK_BOX(welcome->root), Note("Which local broker program will you use?"));
    const char *programs[] = {"TWS", "IB Gateway", NULL};
    welcome->program = gtk_drop_down_new_from_strings(programs);
    gtk_widget_set_name(welcome->program, "broker-welcome-program");
    gtk_widget_set_tooltip_text(welcome->program, "Sets the initial API port. You can check and edit it before connecting.");
    gtk_box_append(GTK_BOX(welcome->root), welcome->program);
    const char *labels[] = {"Continue as _Guest", "Open _Paper account", "Open _Live account", "Local workspace _profiles"};
    const char *names[] = {"broker-welcome-guest", "broker-welcome-paper", "broker-welcome-live", "broker-welcome-profiles"};
    for (unsigned i = 0; i < G_N_ELEMENTS(welcome->buttons); ++i) {
        if (i == 3U && config->openLocalProfiles == NULL) continue;
        GtkWidget *button = gtk_button_new_with_mnemonic(labels[i]);
        welcome->buttons[i] = button;
        gtk_widget_set_name(button, names[i]);
        g_object_set_data(G_OBJECT(button), "broker-choice", GUINT_TO_POINTER(i));
        g_signal_connect(button, "clicked", G_CALLBACK(Choose), welcome);
        gtk_box_append(GTK_BOX(welcome->root), button);
        if (i < 3U)
            gtk_box_append(GTK_BOX(welcome->root),
                Note(UmiBrokerAccessAuthenticationGuide((UmiBrokerAccessMode)i)));
    }
    gtk_box_append(GTK_BOX(welcome->root), Note(
        "Current broker access is read-only. The chart workspace uses simulation; "
        "broker observations appear in the separate connection window. Orders are not transmitted."));
#endif

    /* Account selection is a presentation of the Framework access plan. Merely
     * switching tabs never opens a socket or grants permission to trade. */
    gtk_box_append(GTK_BOX(welcome->root), UmiGtkBrandCreate(TRUE, 44));
    GtkWidget *title = Note("Umicom Trader");
    gtk_widget_add_css_class(title, "title-1");
    gtk_box_append(GTK_BOX(welcome->root), title);
    gtk_box_append(GTK_BOX(welcome->root), Note("Choose an account to get started."));
    welcome->modes = gtk_stack_new();
    gtk_widget_set_name(welcome->modes, "broker-welcome-modes");
    gtk_stack_set_vhomogeneous(GTK_STACK(welcome->modes), FALSE);
    gtk_stack_set_transition_type(GTK_STACK(welcome->modes), GTK_STACK_TRANSITION_TYPE_NONE);
    GtkWidget *switcher = gtk_stack_switcher_new();
    gtk_stack_switcher_set_stack(GTK_STACK_SWITCHER(switcher), GTK_STACK(welcome->modes));
    gtk_widget_set_halign(switcher, GTK_ALIGN_FILL);
    gtk_box_append(GTK_BOX(welcome->root), switcher);
    gtk_box_append(GTK_BOX(welcome->root), welcome->modes);
    const char *titles[] = {"Umicom Demo", "IBKR Paper", "IBKR Live"};
    const char *pages[] = {"demo", "paper", "live"};
    const char *labels[] = {"Start Umicom _Demo", "Continue to _Paper connection", "Continue to _Live connection"};
    const char *names[] = {"broker-welcome-guest", "broker-welcome-paper", "broker-welcome-live"};
    const char *descriptions[] = {
        "Practise with simulated prices and orders on this computer. No broker account is needed.",
        "Use your Interactive Brokers paper account. Sign in to Paper Trading in TWS or IB Gateway and complete its authentication.",
        "Use your Interactive Brokers live account. Sign in to Live Trading in TWS or IB Gateway and complete its authentication."
    };
    /* Live/Paper/Demo are display positions, not enum values. Preserve the
     * established mode IDs so saved plans and connection ports remain stable. */
    const unsigned displayOrder[] = {2U, 1U, 0U};
    for (size_t position = 0U; position < G_N_ELEMENTS(displayOrder); ++position) {
        unsigned i = displayOrder[position];
        GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 14);
        gtk_widget_set_margin_top(page, 18);
        gtk_widget_set_margin_bottom(page, 12);
        gtk_box_append(GTK_BOX(page), Note(descriptions[i]));
        if (i != 0U) {
            GtkWidget *limit = Note("Broker access in this release is read-only. Orders are not sent to Interactive Brokers.");
            gtk_widget_add_css_class(limit, "dim-label");
            gtk_box_append(GTK_BOX(page), limit);
        }
        GtkWidget *button = gtk_button_new_with_mnemonic(labels[i]);
        welcome->buttons[i] = button;
        gtk_widget_set_name(button, names[i]);
        gtk_widget_add_css_class(button, "suggested-action");
        g_object_set_data(G_OBJECT(button), "broker-choice", GUINT_TO_POINTER(i));
        g_signal_connect(button, "clicked", G_CALLBACK(Choose), welcome);
        gtk_box_append(GTK_BOX(page), button);
        gtk_stack_add_titled(GTK_STACK(welcome->modes), page, pages[i], titles[i]);
    }
    welcome->connectionOptions = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_set_name(welcome->connectionOptions, "broker-welcome-connection-options");
    gtk_box_append(GTK_BOX(welcome->connectionOptions), Note("Local broker application"));
    const char *programs[] = {"TWS", "IB Gateway", NULL};
    welcome->program = gtk_drop_down_new_from_strings(programs);
    gtk_widget_set_name(welcome->program, "broker-welcome-program");
    gtk_accessible_update_property(GTK_ACCESSIBLE(welcome->program),
        GTK_ACCESSIBLE_PROPERTY_LABEL, "Local broker application", -1);
    gtk_widget_set_tooltip_text(welcome->program, "Choose the application you signed in to. Connection settings follow on the next screen.");
    gtk_box_append(GTK_BOX(welcome->connectionOptions), welcome->program);
    gtk_box_append(GTK_BOX(welcome->connectionOptions), Note(
        "Your broker password and authentication code are entered in the local broker application. "
        "Umicom Trader connects to that session; it does not store those credentials."));
    gtk_box_append(GTK_BOX(welcome->root), welcome->connectionOptions);
    if (config->openLocalProfiles != NULL) {
        GtkWidget *button = gtk_button_new_with_mnemonic("Open saved _workspace");
        welcome->buttons[3] = button;
        gtk_widget_set_name(button, "broker-welcome-profiles");
        gtk_widget_add_css_class(button, "flat");
        g_object_set_data(G_OBJECT(button), "broker-choice", GUINT_TO_POINTER(3U));
        g_signal_connect(button, "clicked", G_CALLBACK(Choose), welcome);
        gtk_box_append(GTK_BOX(welcome->root), button);
    }
    welcome->status = Note("");
    gtk_widget_set_name(welcome->status, "broker-welcome-status");
    gtk_box_append(GTK_BOX(welcome->root), welcome->status);
    gtk_stack_set_visible_child_name(GTK_STACK(welcome->modes), "paper");
    g_signal_connect(welcome->modes, "notify::visible-child-name", G_CALLBACK(ModeChanged), welcome);
#endif
    BuildSessionLayout(welcome);
    *outWelcome = welcome;
    return UMI_STATUS_OK;
}

GtkWidget *UmiBrokerWelcomeGtkWidget(UmiBrokerWelcomeGtk *welcome)
{
    return welcome != NULL && !welcome->closed ? welcome->root : NULL;
}

void UmiBrokerWelcomeGtkDestroy(UmiBrokerWelcomeGtk *welcome)
{
    if (welcome == NULL || welcome->closed) return;
    welcome->closed = TRUE;
    UmiIbkrGatewayGtkDestroy(welcome->gateway);
    welcome->gateway = NULL;
    g_signal_handlers_disconnect_by_data(welcome->modes, welcome);
    for (size_t i = 0U; i < G_N_ELEMENTS(welcome->begin); ++i)
        g_signal_handlers_disconnect_by_data(welcome->begin[i], welcome);
    g_signal_handlers_disconnect_by_data(welcome->back, welcome);
    /* Disconnect before releasing widgets so retained controls cannot call a
     * retired product owner. No credentials or connection survive this owner. */
    for (size_t i = 0; i < G_N_ELEMENTS(welcome->buttons); ++i)
        if (welcome->buttons[i] != NULL)
            g_signal_handlers_disconnect_by_data(welcome->buttons[i], welcome);
    Release(welcome);
}
