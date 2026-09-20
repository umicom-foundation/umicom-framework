/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/desk_gtk4.c
 *
 * PURPOSE:
 *   Render the Framework-owned Umicom Desk outer shell: global command area,
 *   active workbench canvas, application taskbar and semantic layout strip.
 *
 * DESIGN:
 *   This adapter translates copied Framework snapshots into GTK4 widgets. It
 *   does not scan repositories, launch programs directly, own application
 *   policy or serialise GTK widget trees as layouts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/application_filter.h"
#include "umicom/ui/gtk4/desk.h"
#include "umicom/ui/gtk4/workstation/shell_header.h"
#include "umicom/ui/gtk4/workstation/window_fit.h"
#include "umicom/ui/gtk4/workstation/command_bar.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/application/resource_catalogue.h"

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* These retained widgets are a presentation cache, not a second application
 * catalogue. Every action revalidates against the borrowed Desk runtime. */
typedef struct DeskHomeTile {
    char application_id[UMI_APPLICATION_RUNTIME_ID_CAPACITY];
    GtkWidget *row;
    GtkWidget *icon;
    GtkWidget *name;
    GtkWidget *state;
    GtkWidget *detail;
    GtkWidget *open;
    GtkWidget *select;
    char *search_text;
    bool seen;
    UmiApplicationLaunchChoice choice;
} DeskHomeTile;

/* Copy delayed launch identity so refresh cannot invalidate an emitting row. */
typedef struct DeskPendingApplication {
    UmiGtk4Desk *desk;
    char application_id[UMI_APPLICATION_RUNTIME_ID_CAPACITY];
} DeskPendingApplication;

/* A header can outlive its Desk. This widget-owned binding becomes inert when
 * the borrowed controller is destroyed, without retaining either owner. */
typedef struct DeskHeaderBinding {
    UmiGtk4Desk *desk;
} DeskHeaderBinding;

struct UmiGtk4Desk {
    GtkApplication *application;
    GtkWidget *window;
    GtkWidget *root;
    GtkWidget *content_identity;
    GtkWidget *application_strip;
    GtkWidget *layout_strip;
    GtkWidget *content_stack;
    GtkWidget *application_choices;
    GtkWidget *selection_summary;
    GtkWidget *launch_selected_button;
    GtkWidget *workbench_title;
    GtkWidget *workbench_description;
    GtkWidget *status_label;
    UmiGtk4WorkstationCommandBar *navigation;
    UmiDeskRuntime *runtime;
    GtkWidget *home_search;
    GtkWidget *home_tiles;
    GtkWidget *home_summary;
    GtkWidget *home_empty;
    GPtrArray *home_entries;
    GPtrArray *header_bindings;
    char *home_query;
    UmiDeskApplicationFilter home_filter;
    GArray *choice_cache;
    GArray *strip_cache;
    GArray *layout_cache;
    bool cache_valid;
    bool updating_home;
    bool in_application_action;
    bool destroy_requested;
    guint pending_application_id;
};

/* Home presentation is defined after the existing chooser callbacks. */
static UmiStatus refresh_home(UmiGtk4Desk *desk, const GArray *choices);
static void filter_home(UmiGtk4Desk *desk);
static void set_status(UmiGtk4Desk *desk, const char *message);

/* Header widget finalisation releases its binding and removes a live observer. */
static void free_header_binding(gpointer data)
{
    DeskHeaderBinding *binding = data;
    if (binding->desk != NULL && binding->desk->header_bindings != NULL)
        (void)g_ptr_array_remove_fast(binding->desk->header_bindings, binding);
    g_free(binding);
}

/* All titlebar openings share the same runtime as Home and the taskbar. A
 * declined new-window request never falls back to an unmanaged second process. */
static UmiStatus open_from_bound_header(const char *application_id,
    UmiGtk4WorkstationApplicationOpenMode mode, void *data)
{
    DeskHeaderBinding *binding = data;
    UmiGtk4Desk *desk = binding != NULL ? binding->desk : NULL;
    UmiStatus status;
    if (desk == NULL || desk->destroy_requested) return UMI_STATUS_INVALID_STATE;
    if (application_id == NULL || application_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (mode == UMI_GTK4_WORKSTATION_APPLICATION_OPEN_NEW_WINDOW) return UMI_STATUS_NOT_IMPLEMENTED;
    if (mode != UMI_GTK4_WORKSTATION_APPLICATION_OPEN_STANDARD) return UMI_STATUS_INVALID_ARGUMENT;
    if (desk->in_application_action || desk->pending_application_id != 0U) return UMI_STATUS_BUSY;
    desk->in_application_action = true;
    status = umi_desk_runtime_request_application(desk->runtime, application_id,
        UMI_DESKTOP_APPLICATION_STRIP_LAUNCH_OR_ACTIVATE);
    desk->in_application_action = false;
    if (desk->destroy_requested) {
        umi_gtk4_desk_destroy(desk);
        return status;
    }
    (void)umi_gtk4_desk_refresh(desk);
    set_status(desk, status == UMI_STATUS_OK ? "Open request accepted." : umi_status_text(status));
    return status;
}

/* A caller may retain an old chooser row or the original body after a refresh
 * or window replacement. Disconnect borrowed Desk callbacks before release. */
static void disconnect_desk_widgets(GtkWidget *widget, UmiGtk4Desk *desk)
{
    GtkWidget *child;
    if (widget == NULL) return;
    g_signal_handlers_disconnect_by_data(widget, desk);
    for (child = gtk_widget_get_first_child(widget); child != NULL;
         child = gtk_widget_get_next_sibling(child))
        disconnect_desk_widgets(child, desk);
}

/* Provide the clear box operation used by this module and its client applications. */
static void clear_box(GtkWidget *box, UmiGtk4Desk *desk)
{
    GtkWidget *child;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (box == NULL) return;
    child = gtk_widget_get_first_child(box);
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        disconnect_desk_widgets(child, desk);
        gtk_box_remove(GTK_BOX(box), child);
        child = next;
    }
}

/* Provide the set status operation used by this module and its client applications. */
static void set_status(UmiGtk4Desk *desk, const char *message)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (desk == NULL || desk->status_label == NULL) return;
    gtk_label_set_text(GTK_LABEL(desk->status_label),
                       message != NULL ? message : "");
}

/*
 * Provide the make global button operation used by this module and its client
 * applications.
 */
static GtkWidget *make_global_button(const char *label)
{
    GtkWidget *button = gtk_button_new_with_label(label);
    gtk_widget_add_css_class(button, "flat");
    gtk_widget_add_css_class(button, "umicom-desk-global-button");
    return button;
}

/*
 * Provide the show application chooser operation used by this module and its client
 * applications.
 */
static void show_application_chooser(UmiGtk4Desk *desk)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (desk == NULL || desk->content_stack == NULL) return;
    gtk_stack_set_visible_child_name(
        GTK_STACK(desk->content_stack), "applications");
}

/* Home navigation does not start applications or change saved layouts. */
UmiStatus umi_gtk4_desk_show_home(UmiGtk4Desk *desk)
{
    if (desk == NULL || desk->content_stack == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    gtk_stack_set_visible_child_name(GTK_STACK(desk->content_stack), "home");
    return UMI_STATUS_OK;
}

/* Report actual stack selection without exposing mutable widget ownership. */
const char *umi_gtk4_desk_visible_page(const UmiGtk4Desk *desk)
{
    return desk != NULL && desk->content_stack != NULL
        ? gtk_stack_get_visible_child_name(GTK_STACK(desk->content_stack)) : NULL;
}

/* A stable global Home button returns to applications without resetting search. */
static void on_home_clicked(GtkButton *button, gpointer user_data)
{
    (void)button;
    (void)umi_gtk4_desk_show_home(user_data);
}

/*
 * Provide the refresh selection controls operation used by this module and its client
 * applications.
 */
static UmiStatus refresh_selection_controls(UmiGtk4Desk *desk)
{
    UmiApplicationLaunchSelectionSnapshot snapshot;
    UmiApplicationLaunchReadinessSummary portfolio;
    UmiStatus status;
    char selection_text[224U];
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (desk == NULL || desk->runtime == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Update the selection count immediately without rebuilding the chooser
     * and destroying the check button that emitted the current signal. */
    status = umi_application_launch_selection_snapshot(
        umi_desk_runtime_launch_selection(desk->runtime), &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Refresh the read-only portfolio totals beside the current user selection. */
    status = umi_application_launch_readiness_summary(&portfolio);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)snprintf(
        selection_text, sizeof(selection_text),
        "%zu selected · %zu launchable · %zu running · %zu blocked by layout gate · "
        "portfolio %zu/%zu ready",
        snapshot.selected_count,
        snapshot.eligible_count,
        snapshot.running_count,
        snapshot.readiness_blocked_count,
        portfolio.ready_count,
        portfolio.application_count);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (desk->selection_summary != NULL) {
        gtk_label_set_text(
            GTK_LABEL(desk->selection_summary), selection_text);
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (desk->launch_selected_button != NULL) {
        gtk_widget_set_sensitive(
            desk->launch_selected_button,
            snapshot.selected_count > 0U);
    }
    /* Both pages expose the same selection. Updating check states in place
     * must not recursively submit another model command or replace focus. */
    desk->updating_home = true;
    for (guint index = 0U; desk->home_entries != NULL && index < desk->home_entries->len; ++index) {
        DeskHomeTile *tile = g_ptr_array_index(desk->home_entries, index);
        UmiApplicationLaunchChoice choice;
        if (umi_application_launch_selection_find(umi_desk_runtime_launch_selection(desk->runtime),
                tile->application_id, &choice) == UMI_STATUS_OK)
            gtk_check_button_set_active(GTK_CHECK_BUTTON(tile->select), choice.selected);
    }
    for (GtkWidget *row = desk->application_choices != NULL
            ? gtk_widget_get_first_child(desk->application_choices) : NULL;
         row != NULL; row = gtk_widget_get_next_sibling(row)) {
        GtkWidget *check = gtk_widget_get_first_child(row);
        UmiApplicationLaunchChoice choice;
        const char *id = check != NULL
            ? g_object_get_data(G_OBJECT(check), "umicom-application-id") : NULL;
        if (GTK_IS_CHECK_BUTTON(check) && id != NULL &&
            umi_application_launch_selection_find(umi_desk_runtime_launch_selection(desk->runtime), id, &choice) == UMI_STATUS_OK)
            gtk_check_button_set_active(GTK_CHECK_BUTTON(check), choice.selected);
    }
    desk->updating_home = false;
    return UMI_STATUS_OK;
}

/*
 * Provide the on show applications clicked operation used by this module and its client
 * applications.
 */
static void on_show_applications_clicked(
    GtkButton *button,
    gpointer user_data)
{
    (void)button;
    show_application_chooser((UmiGtk4Desk *)user_data);
}

/*
 * Provide the on launch choice toggled operation used by this module and its client
 * applications.
 */
static void on_launch_choice_toggled(
    GtkCheckButton *button,
    gpointer user_data)
{
    UmiGtk4Desk *desk = (UmiGtk4Desk *)user_data;
    const char *application_id;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (desk == NULL || desk->updating_home) return;
    application_id = (const char *)g_object_get_data(
        G_OBJECT(button), "umicom-application-id");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (application_id == NULL) return;
    status = umi_desk_runtime_select_application(
        desk->runtime,
        application_id,
        gtk_check_button_get_active(button));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = refresh_selection_controls(desk);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        set_status(desk, umi_status_text(status));
    }
}

/*
 * Provide the on select all clicked operation used by this module and its client
 * applications.
 */
static void on_select_all_clicked(GtkButton *button, gpointer user_data)
{
    UmiGtk4Desk *desk = (UmiGtk4Desk *)user_data;
    UmiStatus status;
    (void)button;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (desk == NULL) return;
    status = umi_desk_runtime_select_all_applications(desk->runtime);
    (void)umi_gtk4_desk_refresh(desk);
    set_status(desk, umi_status_text(status));
}

/*
 * Provide the on clear selection clicked operation used by this module and its client
 * applications.
 */
static void on_clear_selection_clicked(
    GtkButton *button,
    gpointer user_data)
{
    UmiGtk4Desk *desk = (UmiGtk4Desk *)user_data;
    UmiStatus status;
    (void)button;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (desk == NULL) return;
    status = umi_desk_runtime_clear_application_selection(desk->runtime);
    (void)umi_gtk4_desk_refresh(desk);
    set_status(desk, umi_status_text(status));
}

/*
 * Provide the on launch selected clicked operation used by this module and its client
 * applications.
 */
static void on_launch_selected_clicked(
    GtkButton *button,
    gpointer user_data)
{
    UmiGtk4Desk *desk = (UmiGtk4Desk *)user_data;
    UmiApplicationLaunchSelectionReport report = {0};
    UmiStatus status;
    char message[256U];
    (void)button;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (desk == NULL || desk->destroy_requested || desk->in_application_action ||
        desk->pending_application_id != 0U) return;
    desk->in_application_action = true;
    status = umi_desk_runtime_launch_selected_applications(
        desk->runtime, &report);
    desk->in_application_action = false;
    /* The same governed adapter is used by Home and multi-selection. A close
     * requested from either adapter callback must finish before freeing Desk. */
    if (desk->destroy_requested) {
        umi_gtk4_desk_destroy(desk);
        return;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (report.result_count == 0U) {
        set_status(desk, "Choose at least one application to launch.");
        return;
    }
    (void)snprintf(
        message, sizeof(message),
        "%zu started · %zu activated · %zu failed",
        report.started_count,
        report.activated_count,
        report.failed_count);
    (void)umi_gtk4_desk_refresh(desk);
    set_status(desk, message);
    /* Apply this branch only when its contract condition is satisfied. */
    if ((report.started_count + report.activated_count) > 0U &&
        desk->content_stack != NULL) {
        gtk_stack_set_visible_child_name(
            GTK_STACK(desk->content_stack), "workbench");
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        g_printerr("Application launch selection: %s\n",
                   umi_status_text(status));
    }
}

/* Run one copied application request after the click signal has unwound. */
static gboolean dispatch_application_from_idle(gpointer user_data)
{
    DeskPendingApplication *pending = user_data;
    UmiGtk4Desk *desk = pending->desk;
    UmiStatus status;
    char message[UMI_APPLICATION_RUNTIME_ID_CAPACITY + 96U];
    desk->pending_application_id = 0U;
    desk->in_application_action = true;
    status = umi_desk_runtime_request_application(
        desk->runtime, pending->application_id,
        UMI_DESKTOP_APPLICATION_STRIP_LAUNCH_OR_ACTIVATE);
    desk->in_application_action = false;
    /* The governed adapter may ask its Desk owner to close during dispatch.
     * Release only after that callback returns, with no later widget access. */
    if (desk->destroy_requested) {
        umi_gtk4_desk_destroy(desk);
        return G_SOURCE_REMOVE;
    }
    (void)umi_gtk4_desk_refresh(desk);
    (void)snprintf(message, sizeof(message), "%s: %s", pending->application_id,
        status == UMI_STATUS_OK ? "Open request accepted." : umi_status_text(status));
    set_status(desk, message);
    return G_SOURCE_REMOVE;
}

/* Queue the existing governed launch action; never execute processes in GTK.
 * Identity is copied and a pending operation is cancelled on Desk destruction. */
static void on_application_clicked(GtkButton *button, gpointer user_data)
{
    UmiGtk4Desk *desk = user_data;
    const char *id = g_object_get_data(G_OBJECT(button), "umicom-application-id");
    DeskPendingApplication *pending;
    if (desk == NULL || id == NULL || !gtk_widget_get_sensitive(GTK_WIDGET(button)) ||
        desk->destroy_requested || desk->in_application_action || desk->pending_application_id != 0U)
        return;
    pending = g_try_new0(DeskPendingApplication, 1);
    if (pending == NULL) { set_status(desk, umi_status_text(UMI_STATUS_OUT_OF_MEMORY)); return; }
    pending->desk = desk;
    if (g_strlcpy(pending->application_id, id, sizeof(pending->application_id)) >= sizeof(pending->application_id)) {
        g_free(pending); set_status(desk, umi_status_text(UMI_STATUS_CAPACITY_EXCEEDED)); return;
    }
    desk->pending_application_id = g_idle_add_full(G_PRIORITY_DEFAULT_IDLE,
        dispatch_application_from_idle, pending, g_free);
    if (desk->pending_application_id == 0U) g_free(pending);
}

/* Provide the on layout clicked operation used by this module and its client applications. */
static void on_layout_clicked(GtkButton *button, gpointer user_data)
{
    UmiGtk4Desk *desk = (UmiGtk4Desk *)user_data;
    const char *layout_id;
    UmiStatus status;
    char message[256U];
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (desk == NULL) return;
    layout_id = (const char *)g_object_get_data(
        G_OBJECT(button), "umicom-layout-id");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout_id == NULL) return;
    status = umi_desk_runtime_activate_layout(desk->runtime, layout_id);
    (void)snprintf(message, sizeof(message), "Layout %s: %s",
                   layout_id, umi_status_text(status));
    set_status(desk, message);
    (void)umi_gtk4_desk_refresh(desk);
}

/*
 * Provide the make application button operation used by this module and its client
 * applications.
 */
static GtkWidget *make_application_button(
    UmiGtk4Desk *desk,
    const UmiDesktopApplicationStripItem *item)
{
    GtkWidget *button = gtk_button_new();
    GtkWidget *content = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *icon = gtk_image_new_from_icon_name(
        item->icon_name[0] != '\0'
            ? item->icon_name
            : "application-x-executable-symbolic");
    GtkWidget *label = gtk_label_new(item->display_name);
    GtkWidget *state = gtk_label_new(
        item->attention ? "!" : (item->running ? "•" : ""));
    char tooltip[512U];

    gtk_image_set_pixel_size(GTK_IMAGE(icon), 20);
    gtk_label_set_ellipsize(GTK_LABEL(label), PANGO_ELLIPSIZE_END);
    gtk_widget_set_hexpand(label, FALSE);
    gtk_widget_add_css_class(button, "flat");
    gtk_widget_add_css_class(button, "umicom-desk-application-button");
    gtk_widget_add_css_class(state, "umicom-desk-application-state");
    /* Apply this operation only while the related capability or state is available. */
    if (item->active) gtk_widget_add_css_class(button, "active");
    /* Apply this branch only when its contract condition is satisfied. */
    if (item->running) gtk_widget_add_css_class(button, "running");
    /* Apply this branch only when its contract condition is satisfied. */
    if (item->pinned) gtk_widget_add_css_class(button, "pinned");
    /* Apply this branch only when its contract condition is satisfied. */
    if (item->attention) {
        gtk_widget_add_css_class(button, "requires-attention");
    }

    gtk_box_append(GTK_BOX(content), icon);
    gtk_box_append(GTK_BOX(content), label);
    gtk_box_append(GTK_BOX(content), state);
    gtk_button_set_child(GTK_BUTTON(button), content);

    (void)snprintf(tooltip, sizeof(tooltip),
                   "%s · %s%s%s",
                   item->display_name,
                   umi_application_runtime_state_text(item->state),
                   item->pinned ? " · pinned" : "",
                   item->active ? " · active" : "");
    gtk_widget_set_tooltip_text(button, tooltip);
    gtk_accessible_update_property(
        GTK_ACCESSIBLE(button),
        GTK_ACCESSIBLE_PROPERTY_LABEL,
        item->display_name,
        -1);
    g_object_set_data_full(
        G_OBJECT(button),
        "umicom-application-id",
        g_strdup(item->application_id),
        g_free);
    g_signal_connect(
        button, "clicked", G_CALLBACK(on_application_clicked), desk);
    return button;
}

/*
 * Provide the make layout button operation used by this module and its client
 * applications.
 */
static GtkWidget *make_layout_button(
    UmiGtk4Desk *desk,
    const UmiDesktopShellTab *tab)
{
    GtkWidget *button = gtk_button_new_with_label(tab->label);
    gtk_widget_add_css_class(button, "flat");
    gtk_widget_add_css_class(button, "umicom-desk-layout-button");
    /* Apply this operation only while the related capability or state is available. */
    if (tab->active) gtk_widget_add_css_class(button, "active");
    /* Apply this branch only when its contract condition is satisfied. */
    if (tab->dirty) gtk_widget_add_css_class(button, "dirty");
    /* Apply this branch only when its contract condition is satisfied. */
    if (tab->pinned) gtk_widget_add_css_class(button, "pinned");
    g_object_set_data_full(
        G_OBJECT(button),
        "umicom-layout-id",
        g_strdup(tab->layout_id),
        g_free);
    g_signal_connect(
        button, "clicked", G_CALLBACK(on_layout_clicked), desk);
    return button;
}

/* Navigation opens only existing Desk UI or a registered semantic layout.
 * It does not start applications or claim the placeholder hosts their tools. */
static void on_navigation_activated(const UmiWsCommandBarItem *item, void *data)
{
    UmiGtk4Desk *desk = data;
    UmiStatus status;
    if (desk == NULL || item == NULL || !item->enabled) return;
    if (strcmp(item->item_id, "desk.home") == 0) {
        (void)umi_gtk4_desk_show_home(desk);
        return;
    }
    if (strcmp(item->item_id, "desk.applications") == 0) {
        show_application_chooser(desk);
        set_status(desk, "Choose applications; launching still requires Launch selected.");
        return;
    }
    if (strncmp(item->item_id, "desk.layout.", sizeof("desk.layout.") - 1U) != 0) return;
    status = umi_desk_runtime_activate_layout(desk->runtime, item->command_id);
    if (status == UMI_STATUS_OK) status = umi_gtk4_desk_refresh(desk);
    set_status(desk, status == UMI_STATUS_OK ? "Desk layout selected." : umi_status_text(status));
}

/* Copy only genuine chooser/layout entries into Framework's existing search
 * model. The old query survives refresh, and no application launch is added. */
static UmiStatus refresh_navigation(UmiGtk4Desk *desk, const UmiDeskRuntimeSnapshot *snapshot)
{
    UmiWsCommandBarModel *model = calloc(1U, sizeof(*model));
    UmiDesktopShellModel *shell = umi_desk_runtime_shell(desk->runtime);
    UmiStatus status;
    size_t index;
    if (model == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    umi_ws_command_bar_model_init(model);
    status = umi_ws_command_bar_model_add(model, "desk.home", "Home",
        "Show the desktop application tiles. Nothing starts automatically.", "desk.home",
        "desktop start home tiles", UMI_WS_COMMAND_SCOPE_PANEL, 110U);
    if (status == UMI_STATUS_OK) status = umi_ws_command_bar_model_add(model, "desk.applications", "Applications",
        "Open the application chooser. Nothing starts automatically.", "desk.applications",
        "chooser launcher select applications", UMI_WS_COMMAND_SCOPE_PANEL, 100U);
    for (index = 0U; status == UMI_STATUS_OK && snapshot->has_shell &&
            index < snapshot->shell.tab_count; ++index) {
        UmiDesktopShellTab tab;
        char id[UMI_UI_ID_CAPACITY];
        int written;
        status = umi_desktop_shell_model_tab_at(shell, index, &tab);
        if (status != UMI_STATUS_OK) break;
        /* The copied command payload retains the complete canonical ID. A
         * bounded presentation key must not shorten a valid long layout ID. */
        written = snprintf(id, sizeof(id), "desk.layout.%zu", index);
        if (written < 0 || (size_t)written >= sizeof(id)) {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }
        status = umi_ws_command_bar_model_add(model, id, tab.label,
            "Select this existing Desk layout. Product panels are not embedded here.",
            tab.layout_id, "layout workspace arrangement", UMI_WS_COMMAND_SCOPE_COMMAND, 70U);
    }
    if (status == UMI_STATUS_OK) status = umi_gtk4_ws_command_bar_set_model(desk->navigation, model);
    free(model);
    return status;
}

/* Build the owned search component before exposing any borrowed Desk callback. */
static UmiStatus create_navigation(UmiGtk4Desk *desk)
{
    UmiWsCommandBarModel *model = calloc(1U, sizeof(*model));
    UmiGtk4WorkstationCommandBarConfig config = umi_gtk4_ws_command_bar_config_default();
    UmiStatus status;
    if (model == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    umi_ws_command_bar_model_init(model);
    config.placeholder = "Search Applications and Desk layouts";
    config.compact_placeholder = "Desk navigation";
    config.initial_available_width = 280;
    status = umi_gtk4_ws_command_bar_create_managed(&config, model, &desk->navigation);
    free(model);
    if (status == UMI_STATUS_OK)
        status = umi_gtk4_ws_command_bar_set_activated_handler(desk->navigation,
            on_navigation_activated, desk);
    return status;
}

/* Provide the build top bar operation used by this module and its client applications. */
static GtkWidget *build_top_bar(UmiGtk4Desk *desk)
{
    GtkWidget *bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    GtkWidget *brand = gtk_label_new("Umicom Desk");
    GtkWidget *search = umi_gtk4_ws_command_bar_widget(desk->navigation);
    GtkWidget *applications = make_global_button("Applications");
    GtkWidget *home = make_global_button("Home");
    gtk_widget_add_css_class(bar, "umicom-desk-global-bar");
    gtk_widget_add_css_class(brand, "title-2");
    gtk_widget_add_css_class(brand, "umicom-desk-content-identity");
    desk->content_identity = brand;
    gtk_widget_set_margin_start(bar, 8);
    gtk_widget_set_margin_end(bar, 8);
    gtk_widget_set_margin_top(bar, 5);
    gtk_widget_set_margin_bottom(bar, 5);
    gtk_box_append(GTK_BOX(bar), brand);
    (void)umi_gtk4_automation_tag_widget(home, "desk.home");
    g_signal_connect(home, "clicked", G_CALLBACK(on_home_clicked), desk);
    gtk_box_append(GTK_BOX(bar), home);
    gtk_box_append(GTK_BOX(bar), make_global_button("File"));
    g_signal_connect(
        applications, "clicked",
        G_CALLBACK(on_show_applications_clicked), desk);
    gtk_box_append(GTK_BOX(bar), applications);
    gtk_box_append(GTK_BOX(bar), make_global_button("Account"));
    gtk_box_append(GTK_BOX(bar), make_global_button("Help"));
    gtk_widget_set_hexpand(search, TRUE);
    gtk_widget_set_halign(search, GTK_ALIGN_END);
    gtk_accessible_update_property(
        GTK_ACCESSIBLE(search),
        GTK_ACCESSIBLE_PROPERTY_LABEL,
        "Search Applications and existing Desk layouts",
        -1);
    gtk_box_append(GTK_BOX(bar), search);
    return bar;
}

/*
 * Provide the make launch choice operation used by this module and its client
 * applications.
 */
static GtkWidget *make_launch_choice(
    UmiGtk4Desk *desk,
    const UmiApplicationLaunchChoice *choice)
{
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *check = gtk_check_button_new();
    GtkWidget *text = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    GtkWidget *name = gtk_label_new(choice->display_name);
    const char *state_text = choice->running
        ? "Running — select to request activation"
        : "Ready to launch";

    /* Explain a blocked workspace directly in the picker when a row is shown. */
    if (!choice->eligible) {
        state_text = choice->readiness_reason[0] != '\0' ? choice->readiness_reason
            : "Temporarily unavailable — clear this selection or retry after refresh";
    }
    GtkWidget *state = gtk_label_new(state_text);

    gtk_widget_add_css_class(row, "umicom-desk-launch-choice");
    gtk_widget_add_css_class(name, "heading");
    gtk_widget_add_css_class(state, "dim-label");
    gtk_widget_set_halign(name, GTK_ALIGN_START);
    gtk_widget_set_halign(state, GTK_ALIGN_START);
    gtk_widget_set_hexpand(text, TRUE);
    gtk_widget_set_sensitive(check, choice->eligible || choice->selected);
    gtk_check_button_set_active(
        GTK_CHECK_BUTTON(check), choice->selected);
    gtk_box_append(GTK_BOX(text), name);
    gtk_box_append(GTK_BOX(text), state);
    gtk_box_append(GTK_BOX(row), check);
    gtk_box_append(GTK_BOX(row), text);
    g_object_set_data_full(
        G_OBJECT(check),
        "umicom-application-id",
        g_strdup(choice->application_id),
        g_free);
    g_signal_connect(
        check, "toggled", G_CALLBACK(on_launch_choice_toggled), desk);
    return row;
}

/*
 * Provide the build application chooser operation used by this module and its client
 * applications.
 */
static GtkWidget *build_application_chooser(UmiGtk4Desk *desk)
{
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    GtkWidget *heading = gtk_label_new("Choose applications to open");
    GtkWidget *description = gtk_label_new(
        "Select one or more installed applications. Each application opens "
        "independently, so you can work in Studio, Trader, Bank and TMS at "
        "the same time.");
    GtkWidget *scroller = gtk_scrolled_window_new();
    GtkWidget *choices = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *select_all = gtk_button_new_with_label("Select all");
    GtkWidget *clear = gtk_button_new_with_label("Clear");
    GtkWidget *launch = gtk_button_new_with_label("Launch selected");
    GtkWidget *summary = gtk_label_new("No applications selected");

    gtk_widget_add_css_class(page, "umicom-desk-application-chooser");
    gtk_widget_add_css_class(heading, "title-1");
    gtk_widget_add_css_class(launch, "suggested-action");
    gtk_widget_add_css_class(summary, "dim-label");
    gtk_label_set_wrap(GTK_LABEL(description), TRUE);
    gtk_widget_set_halign(heading, GTK_ALIGN_START);
    gtk_widget_set_halign(description, GTK_ALIGN_START);
    gtk_widget_set_halign(summary, GTK_ALIGN_START);
    gtk_widget_set_margin_start(page, 48);
    gtk_widget_set_margin_end(page, 48);
    gtk_widget_set_margin_top(page, 36);
    gtk_widget_set_margin_bottom(page, 36);
    gtk_widget_set_hexpand(page, TRUE);
    gtk_widget_set_vexpand(page, TRUE);
    gtk_widget_set_vexpand(scroller, TRUE);
    gtk_scrolled_window_set_policy(
        GTK_SCROLLED_WINDOW(scroller),
        GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroller), choices);
    gtk_widget_set_hexpand(summary, TRUE);
    gtk_box_append(GTK_BOX(actions), select_all);
    gtk_box_append(GTK_BOX(actions), clear);
    gtk_box_append(GTK_BOX(actions), summary);
    gtk_box_append(GTK_BOX(actions), launch);
    gtk_box_append(GTK_BOX(page), heading);
    gtk_box_append(GTK_BOX(page), description);
    gtk_box_append(GTK_BOX(page), gtk_separator_new(
        GTK_ORIENTATION_HORIZONTAL));
    gtk_box_append(GTK_BOX(page), scroller);
    gtk_box_append(GTK_BOX(page), actions);

    g_signal_connect(
        select_all, "clicked", G_CALLBACK(on_select_all_clicked), desk);
    g_signal_connect(
        clear, "clicked", G_CALLBACK(on_clear_selection_clicked), desk);
    g_signal_connect(
        launch, "clicked", G_CALLBACK(on_launch_selected_clicked), desk);
    desk->application_choices = choices;
    desk->selection_summary = summary;
    desk->launch_selected_button = launch;
    return page;
}

/* Release only presentation metadata; GTK owns the corresponding tile widgets. */
static void home_tile_free(gpointer data)
{
    DeskHomeTile *tile = data;
    if (tile == NULL) return;
    g_free(tile->search_text);
    g_free(tile);
}

/* Avoid unnecessary label notifications and accessibility churn during polling. */
static void set_label_if_changed(GtkWidget *label, const char *text)
{
    if (strcmp(gtk_label_get_text(GTK_LABEL(label)), text) != 0)
        gtk_label_set_text(GTK_LABEL(label), text);
}

/* Filter existing rows in place so the search entry and focused controls survive. */
static void filter_home(UmiGtk4Desk *desk)
{
    size_t matches = 0U;
    if (desk == NULL || desk->home_entries == NULL) return;
    for (guint index = 0U; index < desk->home_entries->len; ++index) {
        DeskHomeTile *tile = g_ptr_array_index(desk->home_entries, index);
        bool shown = desk->home_query == NULL || desk->home_query[0] == '\0' ||
            (tile->search_text != NULL && strstr(tile->search_text, desk->home_query) != NULL);
        bool categoryMatches = false;
        if (UmiDeskApplicationFilterMatches(&tile->choice, desk->home_filter,
                &categoryMatches) != UMI_STATUS_OK) categoryMatches = false;
        shown = shown && categoryMatches;
        gtk_widget_set_visible(tile->row, shown);
        if (shown) ++matches;
    }
    if (desk->home_summary != NULL) {
        char message[128];
        (void)snprintf(message, sizeof(message), "%zu of %u applications shown",
            matches, desk->home_entries->len);
        set_label_if_changed(desk->home_summary, message);
    }
    if (desk->home_empty != NULL) {
        /* The earlier text only described search. The shared filter in
         * src/desktop/application_filter.c now adds a category condition.
         * Keep the former call here as a migration reference. */
        // set_label_if_changed(desk->home_empty, desk->home_entries->len == 0U
        // ? "No applications are registered with this desktop."
        // : "No applications match this search. Clear the search to see all applications.");
        set_label_if_changed(desk->home_empty, desk->home_entries->len == 0U
            ? "No applications are registered with this desktop."
            : "No applications match. Choose All applications and clear the search to see the complete list.");
        gtk_widget_set_visible(desk->home_empty, matches == 0U);
    }
}

static void OnHomeFilterChanged(GObject *object, GParamSpec *property, gpointer data)
{
    (void)property;
    UmiGtk4Desk *desk = data;
    guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(object));
    if (desk == NULL || selected > (guint)UMI_DESK_APPLICATION_FILTER_ATTENTION) return;
    desk->home_filter = (UmiDeskApplicationFilter)selected;
    filter_home(desk);
}

/* Search edits alter only tile visibility, never launch or selection state. */
static void on_home_search_changed(GtkSearchEntry *entry, gpointer user_data)
{
    UmiGtk4Desk *desk = user_data;
    char *query = g_utf8_casefold(gtk_editable_get_text(GTK_EDITABLE(entry)), -1);
    if (query == NULL) return;
    g_strstrip(query);
    g_free(desk->home_query);
    desk->home_query = query;
    filter_home(desk);
}

/* Home checkboxes use the existing multi-selection without opening a process. */
static void on_home_select_toggled(GtkCheckButton *button, gpointer user_data)
{
    UmiGtk4Desk *desk = user_data;
    const char *id;
    UmiStatus status;
    if (desk == NULL || desk->updating_home) return;
    id = g_object_get_data(G_OBJECT(button), "umicom-home-application-id");
    if (id == NULL) return;
    status = umi_desk_runtime_select_application(desk->runtime, id,
        gtk_check_button_get_active(button));
    if (status == UMI_STATUS_OK) status = refresh_selection_controls(desk);
    if (status != UMI_STATUS_OK) {
        UmiApplicationLaunchChoice choice;
        if (umi_application_launch_selection_find(
                umi_desk_runtime_launch_selection(desk->runtime), id, &choice) == UMI_STATUS_OK) {
            desk->updating_home = true;
            gtk_check_button_set_active(button, choice.selected);
            desk->updating_home = false;
        }
        set_status(desk, umi_status_text(status));
    }
}

/* Construct one responsive tile using registered identity and semantic icon data. */
static DeskHomeTile *make_home_tile(UmiGtk4Desk *desk, const UmiApplicationLaunchChoice *choice)
{
    DeskHomeTile *tile = g_try_new0(DeskHomeTile, 1);
    GtkWidget *box;
    GtkWidget *frame;
    GtkWidget *heading;
    GtkWidget *actions;
    char tag[UMI_APPLICATION_RUNTIME_ID_CAPACITY + 32U];
    if (tile == NULL) return NULL;
    (void)g_strlcpy(tile->application_id, choice->application_id, sizeof(tile->application_id));
    tile->row = gtk_flow_box_child_new();
    frame = gtk_frame_new(NULL);
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    heading = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    tile->icon = gtk_image_new();
    tile->name = gtk_label_new("");
    tile->state = gtk_label_new("");
    tile->detail = gtk_label_new("");
    tile->open = gtk_button_new_with_label("Launch");
    tile->select = gtk_check_button_new_with_label("Select");
    /* GtkFrame supplies the active theme's border without an assumed card
     * stylesheet, a hardcoded palette or a replacement branding asset. */
    gtk_widget_add_css_class(frame, "umicom-desk-home-tile");
    gtk_widget_set_margin_start(box, 12); gtk_widget_set_margin_end(box, 12);
    gtk_widget_set_margin_top(box, 12); gtk_widget_set_margin_bottom(box, 12);
    gtk_widget_set_size_request(box, 210, -1);
    gtk_image_set_pixel_size(GTK_IMAGE(tile->icon), 32);
    gtk_widget_add_css_class(tile->name, "heading");
    gtk_widget_add_css_class(tile->state, "dim-label");
    gtk_widget_add_css_class(tile->detail, "dim-label");
    gtk_widget_set_hexpand(tile->name, TRUE);
    gtk_label_set_xalign(GTK_LABEL(tile->name), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(tile->name), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(tile->name), 28);
    gtk_label_set_xalign(GTK_LABEL(tile->state), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(tile->state), TRUE);
    gtk_label_set_xalign(GTK_LABEL(tile->detail), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(tile->detail), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(tile->detail), 30);
    gtk_widget_set_vexpand(tile->detail, TRUE);
    gtk_widget_set_valign(tile->detail, GTK_ALIGN_START);
    gtk_widget_set_hexpand(tile->select, TRUE);
    gtk_widget_set_halign(tile->select, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(heading), tile->icon);
    gtk_box_append(GTK_BOX(heading), tile->name);
    gtk_box_append(GTK_BOX(actions), tile->select);
    gtk_box_append(GTK_BOX(actions), tile->open);
    gtk_box_append(GTK_BOX(box), heading);
    gtk_box_append(GTK_BOX(box), tile->state);
    gtk_box_append(GTK_BOX(box), tile->detail);
    gtk_box_append(GTK_BOX(box), actions);
    gtk_frame_set_child(GTK_FRAME(frame), box);
    gtk_flow_box_child_set_child(GTK_FLOW_BOX_CHILD(tile->row), frame);
    g_object_set_data_full(G_OBJECT(tile->open), "umicom-application-id",
        g_strdup(choice->application_id), g_free);
    g_object_set_data_full(G_OBJECT(tile->select), "umicom-home-application-id",
        g_strdup(choice->application_id), g_free);
    (void)snprintf(tag, sizeof(tag), "desk.home.open.%s", choice->application_id);
    (void)umi_gtk4_automation_tag_widget(tile->open, tag);
    (void)snprintf(tag, sizeof(tag), "desk.home.select.%s", choice->application_id);
    (void)umi_gtk4_automation_tag_widget(tile->select, tag);
    g_signal_connect(tile->open, "clicked", G_CALLBACK(on_application_clicked), desk);
    g_signal_connect(tile->select, "toggled", G_CALLBACK(on_home_select_toggled), desk);
    gtk_flow_box_append(GTK_FLOW_BOX(desk->home_tiles), tile->row);
    return tile;
}

/* Presence explanations come from runtime evidence, not product maturity or
 * a guessed executable. Running processes remain distinct from installed files. */
static void update_home_tile(UmiGtk4Desk *desk, DeskHomeTile *tile,
    const UmiApplicationLaunchChoice *choice, const UmiApplicationRuntimeRecord *record)
{
    const UmiApplicationResourceDescriptor *icon = umi_application_resource_catalogue_find(record->icon_resource_id);
    const char *icon_name = icon != NULL && icon->kind == UMI_APPLICATION_RESOURCE_THEME_ICON
        ? icon->locator : "application-x-executable-symbolic";
    const char *state = "Installed";
    const char *detail = "Ready for a governed launch request in its own application window.";
    bool activate = record->running && record->process_token != 0U &&
        record->state != UMI_APPLICATION_RUNTIME_STARTING && record->state != UMI_APPLICATION_RUNTIME_STOPPING &&
        record->enabled && record->compatible &&
        choice->readiness_state == UMI_APPLICATION_LAUNCH_READINESS_READY;
    bool launchable = choice->eligible || activate;
    char *search_source;
    if (record->state == UMI_APPLICATION_RUNTIME_STARTING) {
        state = "Starting"; detail = "A launch is already in progress."; launchable = false;
    } else if (record->state == UMI_APPLICATION_RUNTIME_STOPPING) {
        state = "Stopping"; detail = "Wait for the existing application to stop."; launchable = false;
    } else if (record->running) {
        state = record->installed ? "Running" : "Running · executable missing";
        detail = "Request activation of the existing application. Its process is not embedded in Desk.";
        launchable = activate;
        if (record->process_token == 0U)
            detail = "The runtime reports a running state without a supervised process token. Refresh before requesting activation.";
    } else if (!record->installed) {
        state = "Not installed";
        detail = "The registered application executable is not available. Build or install it, then refresh.";
    } else if (record->state == UMI_APPLICATION_RUNTIME_FAILED) {
        state = "Last launch failed";
        detail = record->last_error[0] != '\0' ? record->last_error : "The last application request failed. You can retry when available.";
    }
    if (!record->enabled) {
        detail = "This application is disabled by its runtime configuration."; launchable = false;
    } else if (!record->compatible) {
        detail = "This application is not compatible with the current runtime."; launchable = false;
    } else if (!launchable && record->installed &&
        choice->readiness_state != UMI_APPLICATION_LAUNCH_READINESS_READY && choice->readiness_reason[0] != '\0') {
        detail = choice->readiness_reason;
    }
    set_label_if_changed(tile->name, choice->display_name);
    set_label_if_changed(tile->state, state);
    set_label_if_changed(tile->detail, detail);
    if (g_strcmp0(gtk_image_get_icon_name(GTK_IMAGE(tile->icon)), icon_name) != 0)
        gtk_image_set_from_icon_name(GTK_IMAGE(tile->icon), icon_name);
    if (g_strcmp0(gtk_button_get_label(GTK_BUTTON(tile->open)), activate ? "Activate" : "Launch") != 0)
        gtk_button_set_label(GTK_BUTTON(tile->open), activate ? "Activate" : "Launch");
    gtk_widget_set_sensitive(tile->open, launchable);
    gtk_widget_set_tooltip_text(tile->open, detail);
    gtk_widget_set_sensitive(tile->select, choice->eligible || choice->selected);
    desk->updating_home = true;
    gtk_check_button_set_active(GTK_CHECK_BUTTON(tile->select), choice->selected);
    desk->updating_home = false;
    tile->choice = *choice; /* Copy presentation evidence; the runtime still owns selection. */
    search_source = g_strconcat(choice->display_name, " ", choice->application_id, NULL);
    g_free(tile->search_text);
    tile->search_text = g_utf8_casefold(search_source, -1);
    g_free(search_source);
}

/* Reconcile by canonical ID so a no-op poll does not replace a focused tile. */
static UmiStatus refresh_home(UmiGtk4Desk *desk, const GArray *choices)
{
    UmiApplicationRuntimeRecord record;
    UmiApplicationRuntimeCatalogue *catalogue = umi_desk_runtime_applications(desk->runtime);
    for (guint index = 0U; index < desk->home_entries->len; ++index)
        ((DeskHomeTile *)g_ptr_array_index(desk->home_entries, index))->seen = false;
    for (guint index = 0U; index < choices->len; ++index) {
        const UmiApplicationLaunchChoice *choice = &g_array_index(choices, UmiApplicationLaunchChoice, index);
        DeskHomeTile *tile = NULL;
        UmiStatus status = umi_application_runtime_catalogue_find(catalogue, choice->application_id, &record);
        if (status != UMI_STATUS_OK) return status;
        for (guint previous = 0U; previous < desk->home_entries->len; ++previous) {
            DeskHomeTile *candidate = g_ptr_array_index(desk->home_entries, previous);
            if (strcmp(candidate->application_id, choice->application_id) == 0) { tile = candidate; break; }
        }
        if (tile == NULL) {
            tile = make_home_tile(desk, choice);
            if (tile == NULL) return UMI_STATUS_OUT_OF_MEMORY;
            g_ptr_array_add(desk->home_entries, tile);
        }
        tile->seen = true;
        update_home_tile(desk, tile, choice, &record);
    }
    for (guint index = desk->home_entries->len; index > 0U; --index) {
        DeskHomeTile *tile = g_ptr_array_index(desk->home_entries, index - 1U);
        if (!tile->seen) {
            disconnect_desk_widgets(tile->row, desk);
            gtk_flow_box_remove(GTK_FLOW_BOX(desk->home_tiles), tile->row);
            g_ptr_array_remove_index(desk->home_entries, index - 1U);
        }
    }
    filter_home(desk);
    return UMI_STATUS_OK;
}

/* Home is a desktop launch surface, not an operating-system kernel or a claim
 * that native product windows have already been embedded inside Desk. */
static GtkWidget *build_home(UmiGtk4Desk *desk)
{
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    GtkWidget *heading = gtk_label_new("Desktop Home");
    GtkWidget *description = gtk_label_new(
        "Find an application to launch or activate. Select several applications, then open Applications to launch your selection together.");
    GtkWidget *scroller = gtk_scrolled_window_new();
    GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *chooser = gtk_button_new_with_label("Open application selection");
    desk->home_search = gtk_search_entry_new();
    desk->home_tiles = gtk_flow_box_new();
    desk->home_summary = gtk_label_new("");
    desk->home_empty = gtk_label_new("");
    gtk_widget_add_css_class(page, "umicom-desk-home");
    gtk_widget_add_css_class(heading, "title-1");
    gtk_widget_add_css_class(desk->home_summary, "dim-label");
    gtk_label_set_xalign(GTK_LABEL(heading), 0.0F);
    gtk_label_set_xalign(GTK_LABEL(description), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(description), TRUE);
    gtk_label_set_wrap(GTK_LABEL(desk->home_empty), TRUE);
    gtk_widget_set_margin_start(page, 24); gtk_widget_set_margin_end(page, 24);
    gtk_widget_set_margin_top(page, 20); gtk_widget_set_margin_bottom(page, 20);
    gtk_widget_set_hexpand(page, TRUE); gtk_widget_set_vexpand(page, TRUE);
    gtk_widget_set_hexpand(desk->home_search, TRUE);
    g_object_set(desk->home_search, "placeholder-text", "Search applications by name", NULL);
    (void)umi_gtk4_automation_tag_widget(desk->home_search, "desk.home.search");
    (void)umi_gtk4_automation_tag_widget(desk->home_tiles, "desk.home.tiles");
    (void)umi_gtk4_automation_tag_widget(desk->home_empty, "desk.home.empty");
    (void)umi_gtk4_automation_tag_widget(chooser, "desk.home.selection");
    gtk_flow_box_set_selection_mode(GTK_FLOW_BOX(desk->home_tiles), GTK_SELECTION_NONE);
    gtk_flow_box_set_min_children_per_line(GTK_FLOW_BOX(desk->home_tiles), 1U);
    gtk_flow_box_set_max_children_per_line(GTK_FLOW_BOX(desk->home_tiles), 8U);
    gtk_flow_box_set_homogeneous(GTK_FLOW_BOX(desk->home_tiles), TRUE);
    gtk_flow_box_set_row_spacing(GTK_FLOW_BOX(desk->home_tiles), 10U);
    gtk_flow_box_set_column_spacing(GTK_FLOW_BOX(desk->home_tiles), 10U);
    gtk_widget_set_valign(desk->home_tiles, GTK_ALIGN_START);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroller), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroller), desk->home_tiles);
    gtk_widget_set_vexpand(scroller, TRUE);
    gtk_widget_set_hexpand(desk->home_summary, TRUE);
    gtk_label_set_xalign(GTK_LABEL(desk->home_summary), 0.0F);
    gtk_box_append(GTK_BOX(actions), desk->home_summary);
    static const char *filterNames[] = {
        "All applications", "Available", "Running", "Selected", "Needs attention", NULL
    };
    GtkWidget *filter = gtk_drop_down_new_from_strings(filterNames);
    gtk_widget_set_tooltip_text(filter,
        "Filter the application list. Hidden applications keep their selection and running state.");
    (void)umi_gtk4_automation_tag_widget(filter, "desk.home.filter");
    g_signal_connect(filter, "notify::selected", G_CALLBACK(OnHomeFilterChanged), desk);
    gtk_box_append(GTK_BOX(actions), filter);
    gtk_box_append(GTK_BOX(actions), chooser);
    gtk_box_append(GTK_BOX(page), heading);
    gtk_box_append(GTK_BOX(page), description);
    gtk_box_append(GTK_BOX(page), desk->home_search);
    gtk_box_append(GTK_BOX(page), actions);
    gtk_box_append(GTK_BOX(page), desk->home_empty);
    gtk_box_append(GTK_BOX(page), scroller);
    g_signal_connect(desk->home_search, "search-changed", G_CALLBACK(on_home_search_changed), desk);
    g_signal_connect(chooser, "clicked", G_CALLBACK(on_show_applications_clicked), desk);
    return page;
}

/*
 * Provide the build workbench placeholder operation used by this module and its client
 * applications.
 */
/* The workbench remains an honest summary until native tools can be embedded. */
static GtkWidget *build_workbench_placeholder(UmiGtk4Desk *desk)
{
    GtkWidget *frame = gtk_frame_new(NULL);
    GtkWidget *content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *title = gtk_label_new("Umicom Desk");
    GtkWidget *description = gtk_label_new(
        "Select an application from the taskbar. "
        "Framework-owned panels and layouts will be hosted here.");
    gtk_widget_add_css_class(frame, "umicom-desk-workbench");
    gtk_widget_add_css_class(title, "title-1");
    gtk_label_set_wrap(GTK_LABEL(description), TRUE);
    gtk_label_set_justify(GTK_LABEL(description), GTK_JUSTIFY_CENTER);
    gtk_widget_set_valign(content, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(content, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(content, TRUE);
    gtk_widget_set_vexpand(content, TRUE);
    gtk_box_append(GTK_BOX(content), title);
    gtk_box_append(GTK_BOX(content), description);
    gtk_frame_set_child(GTK_FRAME(frame), content);
    desk->workbench_title = title;
    desk->workbench_description = description;
    return frame;
}

/* Provide the build bottom area operation used by this module and its client applications. */
static GtkWidget *build_bottom_area(UmiGtk4Desk *desk)
{
    GtkWidget *outer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *applications = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    GtkWidget *layouts = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    GtkWidget *status = gtk_label_new("Umicom Desk ready");
    gtk_widget_add_css_class(outer, "umicom-desk-bottom");
    gtk_widget_add_css_class(applications, "umicom-desk-application-strip");
    gtk_widget_add_css_class(layouts, "umicom-desk-layout-strip");
    gtk_widget_add_css_class(status, "dim-label");
    gtk_widget_set_margin_start(applications, 5);
    gtk_widget_set_margin_end(applications, 5);
    gtk_widget_set_margin_top(applications, 4);
    gtk_widget_set_margin_bottom(applications, 4);
    gtk_widget_set_margin_start(layouts, 5);
    gtk_widget_set_margin_end(layouts, 5);
    gtk_widget_set_margin_top(layouts, 2);
    gtk_widget_set_margin_bottom(layouts, 4);
    gtk_widget_set_halign(status, GTK_ALIGN_END);
    gtk_widget_set_margin_end(status, 8);
    gtk_box_append(GTK_BOX(outer), applications);
    gtk_box_append(GTK_BOX(outer), gtk_separator_new(
        GTK_ORIENTATION_HORIZONTAL));
    gtk_box_append(GTK_BOX(outer), layouts);
    gtk_box_append(GTK_BOX(outer), status);
    desk->application_strip = applications;
    desk->layout_strip = layouts;
    desk->status_label = status;
    return outer;
}

/*
 * Initialise gtk4 desk from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_gtk4_desk_create(
    void *native_gtk_application,
    UmiDeskRuntime *runtime,
    UmiGtk4Desk **out_desk)
{
    UmiGtk4Desk *desk;
    GtkWidget *root;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (native_gtk_application == NULL || !GTK_IS_APPLICATION(native_gtk_application) || runtime == NULL ||
        out_desk == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_desk = NULL;
    desk = (UmiGtk4Desk *)calloc(1U, sizeof(*desk));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (desk == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    desk->application = GTK_APPLICATION(native_gtk_application);
    desk->runtime = runtime;
    desk->home_entries = g_ptr_array_new_with_free_func(home_tile_free);
    desk->header_bindings = g_ptr_array_new();
    status = create_navigation(desk);
    if (status != UMI_STATUS_OK) {
        umi_gtk4_desk_destroy(desk);
        return status;
    }
    desk->window = gtk_application_window_new(desk->application);
    /* Keep native and original-body observations valid even when an external
     * window close unparents the content before the Desk owner is destroyed. */
    g_object_ref_sink(desk->window);
    gtk_window_set_title(GTK_WINDOW(desk->window), "Umicom Desk");
    (void)umi_gtk4_ws_apply_window_identity(GTK_WINDOW(desk->window));
    /* Keep Desk's roomy default within the same monitor bounds as its apps. */
    (void)umi_gtk4_ws_window_fit(
        GTK_WINDOW(desk->window), 1480, 900, 960, 600);

    root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    desk->root = g_object_ref_sink(root);
    gtk_widget_add_css_class(root, "umicom-desk-root");
    gtk_box_append(GTK_BOX(root), build_top_bar(desk));
    gtk_box_append(GTK_BOX(root), gtk_separator_new(
        GTK_ORIENTATION_HORIZONTAL));
    {
        GtkWidget *workbench = build_workbench_placeholder(desk);
        GtkWidget *chooser = build_application_chooser(desk);
        GtkWidget *home = build_home(desk);
        GtkWidget *stack = gtk_stack_new();
        gtk_widget_set_hexpand(stack, TRUE);
        gtk_widget_set_vexpand(stack, TRUE);
        gtk_stack_set_transition_type(
            GTK_STACK(stack), GTK_STACK_TRANSITION_TYPE_CROSSFADE);
        gtk_stack_set_hhomogeneous(GTK_STACK(stack), FALSE);
        gtk_stack_set_vhomogeneous(GTK_STACK(stack), FALSE);
        gtk_stack_add_named(GTK_STACK(stack), home, "home");
        gtk_stack_add_named(GTK_STACK(stack), chooser, "applications");
        gtk_stack_add_named(GTK_STACK(stack), workbench, "workbench");
        gtk_stack_set_visible_child_name(
            GTK_STACK(stack), "home");
        desk->content_stack = stack;
        gtk_box_append(GTK_BOX(root), stack);
    }
    gtk_box_append(GTK_BOX(root), gtk_separator_new(
        GTK_ORIENTATION_HORIZONTAL));
    gtk_box_append(GTK_BOX(root), build_bottom_area(desk));
    gtk_window_set_child(GTK_WINDOW(desk->window), root);

    status = umi_gtk4_desk_refresh(desk);
    if (status != UMI_STATUS_OK) {
        umi_gtk4_desk_destroy(desk);
        return status;
    }
    *out_desk = desk;
    return UMI_STATUS_OK;
}

/* Release or reset state held by gtk4 desk so the same storage can be reused safely. */
void umi_gtk4_desk_destroy(UmiGtk4Desk *desk)
{
    if (desk == NULL) return;
    if (desk->in_application_action) { desk->destroy_requested = true; return; }
    if (desk->pending_application_id != 0U) {
        guint source = desk->pending_application_id;
        desk->pending_application_id = 0U;
        (void)g_source_remove(source);
    }
    /* Invalidate widget-owned header observers before destroying any native
     * tree. A retained or still-dispatching header then returns INVALID_STATE. */
    if (desk->header_bindings != NULL) {
        for (guint index = 0U; index < desk->header_bindings->len; ++index)
            ((DeskHeaderBinding *)g_ptr_array_index(desk->header_bindings, index))->desk = NULL;
        g_ptr_array_free(desk->header_bindings, TRUE);
        desk->header_bindings = NULL;
    }
    /* Widgets must stop borrowing the controller before its runtime pointer
     * becomes invalid. Retained descendants remain inert after this call. */
    disconnect_desk_widgets(desk->root, desk);
    disconnect_desk_widgets(desk->window, desk);
    umi_gtk4_ws_command_bar_destroy(desk->navigation);
    desk->navigation = NULL;
    if (desk->window != NULL) {
        gtk_window_destroy(GTK_WINDOW(desk->window));
        g_object_unref(desk->window);
    }
    if (desk->root != NULL) g_object_unref(desk->root);
    if (desk->home_entries != NULL) g_ptr_array_free(desk->home_entries, TRUE);
    if (desk->choice_cache != NULL) g_array_unref(desk->choice_cache);
    if (desk->strip_cache != NULL) g_array_unref(desk->strip_cache);
    if (desk->layout_cache != NULL) g_array_unref(desk->layout_cache);
    g_free(desk->home_query);
    free(desk);
}

/* A topmost identity owner hides only the duplicate product label, never the
 * Desk application launcher, menus, application strip or status information. */
UmiStatus umi_gtk4_desk_set_content_identity_visible(UmiGtk4Desk *desk, bool visible)
{
    if (desk == NULL || desk->content_identity == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    gtk_widget_set_visible(desk->content_identity, visible);
    return UMI_STATUS_OK;
}

/* Replace unmanaged titlebar launching with a widget-owned runtime binding.
 * No strong reference cycle is formed between Desk, header and native window. */
UmiStatus umi_gtk4_desk_bind_shell_header(
    UmiGtk4Desk *desk, UmiGtk4WorkstationShellHeader *header)
{
    GtkWidget *widget;
    DeskHeaderBinding *binding;
    UmiStatus status;
    if (desk == NULL || header == NULL || desk->destroy_requested)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (desk->in_application_action) return UMI_STATUS_BUSY;
    widget = umi_gtk4_ws_shell_header_widget(header);
    if (widget == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    binding = g_object_get_data(G_OBJECT(widget), "umicom-desk-runtime-binding");
    if (binding != NULL && binding->desk == desk)
        return umi_gtk4_ws_shell_header_set_application_open_handler(header, open_from_bound_header, binding);
    binding = g_try_new0(DeskHeaderBinding, 1);
    if (binding == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    binding->desk = desk;
    status = umi_gtk4_ws_shell_header_set_application_open_handler(header, open_from_bound_header, binding);
    /* These results are rejected before the setter changes the handler. Other
     * failures can come from its subsequent refresh, so preserve that handler's
     * binding even when availability could not yet be projected. */
    if (status == UMI_STATUS_BUSY || status == UMI_STATUS_INVALID_ARGUMENT) {
        g_free(binding);
        return status;
    }
    g_ptr_array_add(desk->header_bindings, binding);
    g_object_set_data_full(G_OBJECT(widget), "umicom-desk-runtime-binding", binding, free_header_binding);
    return status;
}

/* Compare exact displayed selection fields, ignoring a refresh-only revision.
 * No hash shortcut may leave a changed application showing stale controls. */
static bool choice_views_equal(const GArray *left, const GArray *right)
{
    if (left == NULL || left->len != right->len) return false;
    for (guint index = 0U; index < left->len; ++index) {
        const UmiApplicationLaunchChoice *a = &g_array_index(left, UmiApplicationLaunchChoice, index);
        const UmiApplicationLaunchChoice *b = &g_array_index(right, UmiApplicationLaunchChoice, index);
        if (strcmp(a->application_id, b->application_id) != 0 || strcmp(a->display_name, b->display_name) != 0 ||
            strcmp(a->readiness_reason, b->readiness_reason) != 0 || strcmp(a->icon_resource_id, b->icon_resource_id) != 0 ||
            strcmp(a->layout_id, b->layout_id) != 0 || a->state != b->state || a->selected != b->selected ||
            a->eligible != b->eligible || a->running != b->running || a->readiness_state != b->readiness_state ||
            a->readiness_percent != b->readiness_percent) return false;
    }
    return true;
}

/* Preserve taskbar focus and callbacks when only projection revisions advance. */
static bool strip_views_equal(const GArray *left, const GArray *right)
{
    if (left == NULL || left->len != right->len) return false;
    for (guint index = 0U; index < left->len; ++index) {
        const UmiDesktopApplicationStripItem *a = &g_array_index(left, UmiDesktopApplicationStripItem, index);
        const UmiDesktopApplicationStripItem *b = &g_array_index(right, UmiDesktopApplicationStripItem, index);
        if (strcmp(a->application_id, b->application_id) != 0 || strcmp(a->display_name, b->display_name) != 0 ||
            strcmp(a->icon_name, b->icon_name) != 0 || a->state != b->state || a->pinned != b->pinned ||
            a->running != b->running || a->active != b->active || a->attention != b->attention ||
            a->enabled != b->enabled) return false;
    }
    return true;
}

/* Layout navigation changes only when its semantic rows change. */
static bool layout_views_equal(const GArray *left, const GArray *right)
{
    if (left == NULL || left->len != right->len) return false;
    for (guint index = 0U; index < left->len; ++index) {
        const UmiDesktopShellTab *a = &g_array_index(left, UmiDesktopShellTab, index);
        const UmiDesktopShellTab *b = &g_array_index(right, UmiDesktopShellTab, index);
        if (strcmp(a->tab_id, b->tab_id) != 0 || strcmp(a->layout_id, b->layout_id) != 0 ||
            strcmp(a->label, b->label) != 0 || strcmp(a->category, b->category) != 0 ||
            a->active != b->active || a->pinned != b->pinned || a->dirty != b->dirty || a->closable != b->closable)
            return false;
    }
    return true;
}

/* Stage bounded copied views, then reconcile only changed native presentation.
 * Runtime state remains authoritative; cache records never execute actions. */
UmiStatus umi_gtk4_desk_refresh(UmiGtk4Desk *desk)
{
    UmiDeskRuntimeSnapshot snapshot;
    UmiDesktopApplicationStrip *strip;
    UmiApplicationLaunchSelection *launch_selection;
    size_t index;
    UmiStatus status;
    GArray *choices = NULL;
    GArray *items = NULL;
    GArray *tabs = NULL;
    bool choices_changed;
    bool strip_changed;
    bool layouts_changed;
    char status_text[256U];
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (desk == NULL || desk->runtime == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_desk_runtime_refresh(desk->runtime);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_desk_runtime_snapshot(desk->runtime, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    if (snapshot.launch_selection.choice_count > UMI_APPLICATION_RUNTIME_MAX_APPLICATIONS ||
        snapshot.strip.item_count > UMI_APPLICATION_RUNTIME_MAX_APPLICATIONS ||
        (snapshot.has_shell && snapshot.shell.tab_count > UMI_DESKTOP_MAX_TABS))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    choices = g_array_sized_new(FALSE, TRUE, sizeof(UmiApplicationLaunchChoice),
        (guint)snapshot.launch_selection.choice_count);
    items = g_array_sized_new(FALSE, TRUE, sizeof(UmiDesktopApplicationStripItem),
        (guint)snapshot.strip.item_count);
    tabs = g_array_sized_new(FALSE, TRUE, sizeof(UmiDesktopShellTab),
        snapshot.has_shell ? (guint)snapshot.shell.tab_count : 0U);
    launch_selection = umi_desk_runtime_launch_selection(desk->runtime);
    strip = umi_desk_runtime_application_strip(desk->runtime);
    for (index = 0U; index < snapshot.launch_selection.choice_count; ++index) {
        UmiApplicationLaunchChoice choice;
        status = umi_application_launch_selection_at(launch_selection, index, &choice);
        if (status != UMI_STATUS_OK) goto finish_refresh;
        g_array_append_val(choices, choice);
    }
    for (index = 0U; index < snapshot.strip.item_count; ++index) {
        UmiDesktopApplicationStripItem item;
        status = umi_desktop_application_strip_at(strip, index, &item);
        if (status != UMI_STATUS_OK) goto finish_refresh;
        g_array_append_val(items, item);
    }
    if (snapshot.has_shell) {
        UmiDesktopShellModel *shell = umi_desk_runtime_shell(desk->runtime);
        for (index = 0U; index < snapshot.shell.tab_count; ++index) {
            UmiDesktopShellTab tab;
            status = umi_desktop_shell_model_tab_at(shell, index, &tab);
            if (status != UMI_STATUS_OK) goto finish_refresh;
            g_array_append_val(tabs, tab);
        }
    }
    choices_changed = !desk->cache_valid || !choice_views_equal(desk->choice_cache, choices);
    strip_changed = !desk->cache_valid || !strip_views_equal(desk->strip_cache, items);
    layouts_changed = !desk->cache_valid || !layout_views_equal(desk->layout_cache, tabs);
    if (layouts_changed) {
        status = refresh_navigation(desk, &snapshot);
        if (status != UMI_STATUS_OK) goto finish_refresh;
    }
    status = refresh_home(desk, choices);
    if (status != UMI_STATUS_OK) goto finish_refresh;
    if (choices_changed) clear_box(desk->application_choices, desk);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         choices_changed && index < choices->len;
         ++index) {
        UmiApplicationLaunchChoice choice = g_array_index(choices, UmiApplicationLaunchChoice, index);
        /* Keep invalid Framework surfaces visible so the user can understand
         * why a product is blocked. A retained unavailable selection must stay
         * visible and removable until the user clears it or the app returns. */
        if (!choice.eligible && !choice.selected &&
            choice.readiness_state == UMI_APPLICATION_LAUNCH_READINESS_READY) {
            continue;
        }
        gtk_box_append(GTK_BOX(desk->application_choices),
                       make_launch_choice(desk, &choice));
    }
    status = refresh_selection_controls(desk);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto finish_refresh;

    if (strip_changed) {
        clear_box(desk->application_strip, desk);
        set_label_if_changed(desk->workbench_title, "Umicom Desk");
        set_label_if_changed(desk->workbench_description,
            "Choose an application from Home or the taskbar. Product windows open separately.");
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; strip_changed && index < items->len; ++index) {
        UmiDesktopApplicationStripItem item = g_array_index(items, UmiDesktopApplicationStripItem, index);
        /* The staged taskbar record has already passed its snapshot read. */
        gtk_box_append(GTK_BOX(desk->application_strip),
                       make_application_button(desk, &item));
        /* Apply this operation only while the related capability or state is available. */
        if (item.active) {
            gtk_label_set_text(GTK_LABEL(desk->workbench_title),
                               item.display_name);
            gtk_label_set_text(
                GTK_LABEL(desk->workbench_description),
                item.running
                    ? "The runtime reports this application as running. Its native window is separate from Desk."
                    : "Application is selected but not running.");
        }
    }

    if (layouts_changed) clear_box(desk->layout_strip, desk);
    /* Apply this branch only when its contract condition is satisfied. */
    if (layouts_changed && snapshot.has_shell) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < snapshot.shell.tab_count; ++index) {
            UmiDesktopShellTab tab = g_array_index(tabs, UmiDesktopShellTab, index);
            /* Keep each current semantic layout available in the native strip. */
            gtk_box_append(GTK_BOX(desk->layout_strip),
                           make_layout_button(desk, &tab));
        }
        {
            GtkWidget *new_layout = gtk_button_new_with_label("+");
            gtk_widget_add_css_class(new_layout, "flat");
            gtk_widget_set_tooltip_text(
                new_layout, "New layout creation is not connected in this Desk view yet.");
            gtk_widget_set_sensitive(new_layout, FALSE);
            gtk_box_append(GTK_BOX(desk->layout_strip), new_layout);
        }
    }

    (void)snprintf(status_text, sizeof(status_text),
                   "%zu applications · %zu running · %zu pinned%s",
                   snapshot.strip.item_count,
                   snapshot.strip.running_count,
                   snapshot.strip.pinned_count,
                   snapshot.strip.attention_count > 0U
                       ? " · attention required" : "");
    if (strip_changed) set_status(desk, status_text);
    if (desk->choice_cache != NULL) g_array_unref(desk->choice_cache);
    if (desk->strip_cache != NULL) g_array_unref(desk->strip_cache);
    if (desk->layout_cache != NULL) g_array_unref(desk->layout_cache);
    desk->choice_cache = choices; choices = NULL;
    desk->strip_cache = items; items = NULL;
    desk->layout_cache = tabs; tabs = NULL;
    desk->cache_valid = true;
    status = UMI_STATUS_OK;
finish_refresh:
    if (choices != NULL) g_array_unref(choices);
    if (items != NULL) g_array_unref(items);
    if (tabs != NULL) g_array_unref(tabs);
    return status;
}

/* Provide the gtk4 desk present operation used by this module and its client applications. */
UmiStatus umi_gtk4_desk_present(UmiGtk4Desk *desk)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (desk == NULL || desk->window == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    gtk_window_present(GTK_WINDOW(desk->window));
    return UMI_STATUS_OK;
}

/*
 * Provide the gtk4 desk native window operation used by this module and its client
 * applications.
 */
void *umi_gtk4_desk_native_window(UmiGtk4Desk *desk)
{
    return desk != NULL ? desk->window : NULL;
}
