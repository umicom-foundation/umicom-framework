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

struct UmiBrokerWelcomeGtk {
    unsigned references;
    gboolean closed, busy;
    UmiBrokerWelcomeGtkConfig config;
    GtkWidget *root, *buttons[4], *status, *program;
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
    gtk_widget_set_margin_start(welcome->root, 24);
    gtk_widget_set_margin_end(welcome->root, 24);
    gtk_widget_set_margin_top(welcome->root, 24);
    gtk_widget_set_margin_bottom(welcome->root, 24);
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
    welcome->status = Note("");
    gtk_widget_set_name(welcome->status, "broker-welcome-status");
    gtk_box_append(GTK_BOX(welcome->root), welcome->status);
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
    /* Disconnect before releasing widgets so retained controls cannot call a
     * retired product owner. No credentials or connection survive this owner. */
    for (size_t i = 0; i < G_N_ELEMENTS(welcome->buttons); ++i)
        if (welcome->buttons[i] != NULL)
            g_signal_handlers_disconnect_by_data(welcome->buttons[i], welcome);
    Release(welcome);
}
