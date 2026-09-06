/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/application_product_application_gtk4.c
 *
 * PURPOSE:
 *   Give thin applications one native startup and shutdown implementation.
 *   Product-specific layouts come from the Framework experience catalogue.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/suite_layout/gtk4_product_application.h"

#include <stdio.h>
#include <string.h>

#include "umicom/application/experience_catalogue.h"
#include "umicom/ui/gtk4/workstation/shell_header.h"
#include "umicom/ui/gtk4/workstation/window_fit.h"

/* GTK owns the window; this state owns the controllers and pending idle task.
 * The whole state stays alive on the caller's stack while GTK runs. */
typedef struct ProductApplicationState {
    UmiApplicationProductGtk4WorkstationConfig config;
    GtkWindow *window;
    UmiApplicationProductGtk4Workstation *workstation;
    UmiGtk4WorkstationStartupSplash *splash;
    guint startup_source;
    int startup_failed;
} ProductApplicationState;

/* Layout review must never report that a payment, render or other product
 * operation succeeded when no domain service is attached. */
static UmiStatus preview_controller(
    void *context,
    const UmiApplicationPresentationPanelPlacement *placement,
    UmiApplicationPresentationSurfaceEvent event,
    const char *payload,
    UmiApplicationPresentationSurfaceUpdate *update)
{
    (void)context;
    (void)payload;
    if (placement == NULL || placement->panel == NULL || update == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    memset(update, 0, sizeof(*update));
    if (event == UMI_APPLICATION_PRESENTATION_EVENT_UNMOUNT ||
        event == UMI_APPLICATION_PRESENTATION_EVENT_DEACTIVATE) {
        update->state = UMI_APPLICATION_PRESENTATION_STATE_DORMANT;
        return UMI_STATUS_OK;
    }
    update->state = UMI_APPLICATION_PRESENTATION_STATE_OFFLINE;
    (void)snprintf(update->message, sizeof(update->message), "%s",
        "Layout available for testing. This product service is not connected yet.");
    (void)snprintf(update->badge, sizeof(update->badge), "%s", "Layout preview");
    /* Reject domain commands even if a future catalogue adds an action button. */
    return event == UMI_APPLICATION_PRESENTATION_EVENT_COMMAND
        ? UMI_STATUS_UNAVAILABLE : UMI_STATUS_OK;
}

/* One explicit offline controller covers every planned product component. */
static UmiStatus register_preview_controllers(
    UmiApplicationPresentationSurfaceRuntime *runtime, void *context)
{
    return umi_application_presentation_surface_runtime_register_controller_for_all(
        runtime, preview_controller, context);
}

/* Forget a finalized window before any pending startup callback can use it. */
static void product_window_finalized(gpointer data, GObject *object)
{
    ProductApplicationState *state = data;
    (void)object;
    state->window = NULL;
    if (state->startup_source != 0U) {
        g_source_remove(state->startup_source);
        state->startup_source = 0U;
    }
}

/* Release presentation controllers only after their host is detached. */
static void product_content_dispose(ProductApplicationState *state)
{
    umi_gtk4_ws_startup_splash_destroy(state->splash);
    state->splash = NULL;
    umi_application_product_gtk4_workstation_destroy(state->workstation);
    state->workstation = NULL;
}

/* Construct the heavier workspace after the startup surface can be drawn.
 * An error stays visible in the same branded window and is also sent to stderr. */
static gboolean product_complete_startup(gpointer data)
{
    ProductApplicationState *state = data;
    UmiStatus status;
    GtkWidget *content;
    state->startup_source = 0U;
    if (state->window == NULL) return G_SOURCE_REMOVE;
    status = umi_application_product_gtk4_workstation_create(
        &state->config, &state->workstation);
    /* Native launch explicitly opts in; no controller constructor creates storage. */
    if (status == UMI_STATUS_OK) {
        UmiStatus storage_status =
            umi_application_product_gtk4_workstation_enable_checkpoint_storage(state->workstation, 1);
        if (storage_status != UMI_STATUS_OK)
            (void)fprintf(stderr, "%s layout storage: %s\n",
                state->config.title, umi_status_text(storage_status));
    }
    content = status == UMI_STATUS_OK
        ? umi_application_product_gtk4_workstation_widget(state->workstation) : NULL;
    if (content == NULL) {
        char message[192];
        if (status == UMI_STATUS_OK) status = UMI_STATUS_INVALID_STATE;
        state->startup_failed = 1;
        (void)snprintf(message, sizeof(message), "Workspace could not open: %s",
            umi_status_text(status));
        (void)umi_gtk4_ws_startup_splash_set_status(state->splash,
            message, "Action required");
        (void)umi_gtk4_ws_startup_splash_set_progress(state->splash, 1.0, 0);
        (void)fprintf(stderr, "%s: %s\n", state->config.title, message);
        return G_SOURCE_REMOVE;
    }
    gtk_window_set_child(state->window, content);
    umi_gtk4_ws_startup_splash_destroy(state->splash);
    state->splash = NULL;
    return G_SOURCE_REMOVE;
}

/* Each product opens its own window. Reactivation focuses that existing
 * window rather than creating a second controller tree behind it. */
static void product_activate(GtkApplication *application, gpointer data)
{
    ProductApplicationState *state = data;
    UmiGtk4WorkstationStartupSplashConfig splash_config;
    UmiStatus status;
    if (state->window != NULL) {
        gtk_window_present(state->window);
        return;
    }
    product_content_dispose(state);
    state->window = GTK_WINDOW(gtk_application_window_new(application));
    gtk_window_set_title(state->window, state->config.title);
    (void)umi_gtk4_ws_apply_window_identity(state->window);
    (void)umi_gtk4_ws_window_fit(state->window, 1180, 760, 720, 480);
    g_object_weak_ref(G_OBJECT(state->window), product_window_finalized, state);
    splash_config = umi_gtk4_ws_startup_splash_config_default(
        state->config.application_id, state->config.title);
    splash_config.subtitle = "Customisable application workspace";
    splash_config.status = "Preparing layouts, panels and appearance…";
    splash_config.mode_badge = state->config.mode_badge;
    status = umi_gtk4_ws_startup_splash_create(&splash_config, &state->splash);
    if (status != UMI_STATUS_OK) {
        state->startup_failed = 1;
        (void)fprintf(stderr, "%s startup: %s\n", state->config.title,
            umi_status_text(status));
        gtk_window_destroy(state->window);
        return;
    }
    gtk_window_set_child(state->window,
        umi_gtk4_ws_startup_splash_widget(state->splash));
    gtk_window_present(state->window);
    state->startup_source = g_idle_add(product_complete_startup, state);
}

/* Keep callbacks, controllers and GTK ownership in one place so every product
 * can reuse the same safe startup path with only an identity and registrar. */
int umi_application_product_gtk4_run(
    const UmiApplicationProductGtk4WorkstationConfig *config,
    int argc,
    char **argv)
{
    ProductApplicationState state = {0};
    const UmiApplicationExperienceDefinition *experience;
    GtkApplication *application;
    char *gtk_id;
    int result;
    if (config == NULL || config->application_id == NULL ||
        config->title == NULL || config->title[0] == '\0' || argc < 0 ||
        (argc > 0 && argv == NULL)) return 1;
    experience = umi_application_experience_catalogue_find(config->application_id);
    if (experience == NULL) {
        (void)fprintf(stderr, "No Framework experience for %s\n", config->application_id);
        return 1;
    }
    state.config = *config;
    state.config.application_id = experience->application_id;
    /* Resolve legacy aliases once before recipe selection and GTK branding. */
    gtk_id = g_strconcat(state.config.application_id, ".gtk", NULL);
    if (!g_application_id_is_valid(gtk_id)) {
        g_free(gtk_id);
        return 1;
    }
    if (state.config.register_controllers == NULL) {
        state.config.register_controllers = register_preview_controllers;
        state.config.mode_badge = "Layout preview";
    }
    application = gtk_application_new(gtk_id, G_APPLICATION_DEFAULT_FLAGS);
    g_free(gtk_id);
    g_signal_connect(application, "activate", G_CALLBACK(product_activate), &state);
    result = g_application_run(G_APPLICATION(application), argc, argv);
    /* Cancel every callback borrowing stack state before returning to main. */
    if (state.startup_source != 0U) g_source_remove(state.startup_source);
    if (state.window != NULL) {
        g_object_weak_unref(G_OBJECT(state.window), product_window_finalized, &state);
        gtk_window_destroy(state.window);
        state.window = NULL;
    }
    product_content_dispose(&state);
    g_object_unref(application);
    return result != 0 ? result : state.startup_failed;
}
