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
#include "umicom/ui/gtk4/desk.h"

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiGtk4Desk {
    GtkApplication *application;
    GtkWidget *window;
    GtkWidget *application_strip;
    GtkWidget *layout_strip;
    GtkWidget *content_stack;
    GtkWidget *application_choices;
    GtkWidget *selection_summary;
    GtkWidget *launch_selected_button;
    GtkWidget *workbench_title;
    GtkWidget *workbench_description;
    GtkWidget *status_label;
    UmiDeskRuntime *runtime;
};

/* Provide the clear box operation used by this module and its client applications. */
static void clear_box(GtkWidget *box)
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

/*
 * Provide the refresh selection controls operation used by this module and its client
 * applications.
 */
static UmiStatus refresh_selection_controls(UmiGtk4Desk *desk)
{
    UmiApplicationLaunchSelectionSnapshot snapshot;
    UmiStatus status;
    char selection_text[160U];
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
    (void)snprintf(
        selection_text, sizeof(selection_text),
        "%zu selected · %zu installed · %zu running",
        snapshot.selected_count,
        snapshot.eligible_count,
        snapshot.running_count);
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
    if (desk == NULL) return;
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
    UmiApplicationLaunchSelectionReport report;
    UmiStatus status;
    char message[256U];
    (void)button;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (desk == NULL) return;
    status = umi_desk_runtime_launch_selected_applications(
        desk->runtime, &report);
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

/*
 * Provide the on application clicked operation used by this module and its client
 * applications.
 */
static void on_application_clicked(GtkButton *button, gpointer user_data)
{
    UmiGtk4Desk *desk = (UmiGtk4Desk *)user_data;
    const char *application_id;
    UmiStatus status;
    char message[256U];
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (desk == NULL) return;
    application_id = (const char *)g_object_get_data(
        G_OBJECT(button), "umicom-application-id");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (application_id == NULL) return;
    status = umi_desk_runtime_request_application(
        desk->runtime, application_id,
        UMI_DESKTOP_APPLICATION_STRIP_LAUNCH_OR_ACTIVATE);
    (void)snprintf(message, sizeof(message), "%s: %s",
                   application_id, umi_status_text(status));
    set_status(desk, message);
    (void)umi_gtk4_desk_refresh(desk);
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

/* Provide the build top bar operation used by this module and its client applications. */
static GtkWidget *build_top_bar(UmiGtk4Desk *desk)
{
    GtkWidget *bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    GtkWidget *brand = gtk_label_new("Umicom Desk");
    GtkWidget *search = gtk_search_entry_new();
    GtkWidget *applications = make_global_button("Applications");
    gtk_widget_add_css_class(bar, "umicom-desk-global-bar");
    gtk_widget_add_css_class(brand, "title-2");
    gtk_widget_set_margin_start(bar, 8);
    gtk_widget_set_margin_end(bar, 8);
    gtk_widget_set_margin_top(bar, 5);
    gtk_widget_set_margin_bottom(bar, 5);
    gtk_box_append(GTK_BOX(bar), brand);
    gtk_box_append(GTK_BOX(bar), make_global_button("File"));
    g_signal_connect(
        applications, "clicked",
        G_CALLBACK(on_show_applications_clicked), desk);
    gtk_box_append(GTK_BOX(bar), applications);
    gtk_box_append(GTK_BOX(bar), make_global_button("Account"));
    gtk_box_append(GTK_BOX(bar), make_global_button("Help"));
    gtk_widget_set_hexpand(search, TRUE);
    gtk_widget_set_halign(search, GTK_ALIGN_END);
    gtk_widget_set_size_request(search, 320, -1);
    gtk_accessible_update_property(
        GTK_ACCESSIBLE(search),
        GTK_ACCESSIBLE_PROPERTY_LABEL,
        "Search applications, commands, layouts and settings",
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
    GtkWidget *state = gtk_label_new(
        choice->running ? "Running — select to bring it forward"
                        : "Ready to launch");

    gtk_widget_add_css_class(row, "umicom-desk-launch-choice");
    gtk_widget_add_css_class(name, "heading");
    gtk_widget_add_css_class(state, "dim-label");
    gtk_widget_set_halign(name, GTK_ALIGN_START);
    gtk_widget_set_halign(state, GTK_ALIGN_START);
    gtk_widget_set_hexpand(text, TRUE);
    gtk_widget_set_sensitive(check, choice->eligible);
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

/*
 * Provide the build workbench placeholder operation used by this module and its client
 * applications.
 */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (native_gtk_application == NULL || runtime == NULL ||
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
    desk->window = gtk_application_window_new(desk->application);
    gtk_window_set_title(GTK_WINDOW(desk->window), "Umicom Desk");
    gtk_window_set_default_size(GTK_WINDOW(desk->window), 1480, 900);

    root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_add_css_class(root, "umicom-desk-root");
    gtk_box_append(GTK_BOX(root), build_top_bar(desk));
    gtk_box_append(GTK_BOX(root), gtk_separator_new(
        GTK_ORIENTATION_HORIZONTAL));
    {
        GtkWidget *workbench = build_workbench_placeholder(desk);
        GtkWidget *chooser = build_application_chooser(desk);
        GtkWidget *stack = gtk_stack_new();
        gtk_widget_set_hexpand(stack, TRUE);
        gtk_widget_set_vexpand(stack, TRUE);
        gtk_stack_set_transition_type(
            GTK_STACK(stack), GTK_STACK_TRANSITION_TYPE_CROSSFADE);
        gtk_stack_add_named(GTK_STACK(stack), chooser, "applications");
        gtk_stack_add_named(GTK_STACK(stack), workbench, "workbench");
        gtk_stack_set_visible_child_name(
            GTK_STACK(stack), "applications");
        desk->content_stack = stack;
        gtk_box_append(GTK_BOX(root), stack);
    }
    gtk_box_append(GTK_BOX(root), gtk_separator_new(
        GTK_ORIENTATION_HORIZONTAL));
    gtk_box_append(GTK_BOX(root), build_bottom_area(desk));
    gtk_window_set_child(GTK_WINDOW(desk->window), root);

    *out_desk = desk;
    return umi_gtk4_desk_refresh(desk);
}

/* Release or reset state held by gtk4 desk so the same storage can be reused safely. */
void umi_gtk4_desk_destroy(UmiGtk4Desk *desk)
{
    free(desk);
}

/* Provide the gtk4 desk refresh operation used by this module and its client applications. */
UmiStatus umi_gtk4_desk_refresh(UmiGtk4Desk *desk)
{
    UmiDeskRuntimeSnapshot snapshot;
    UmiDesktopApplicationStrip *strip;
    UmiApplicationLaunchSelection *launch_selection;
    size_t index;
    UmiStatus status;
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

    launch_selection = umi_desk_runtime_launch_selection(desk->runtime);
    clear_box(desk->application_choices);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < snapshot.launch_selection.choice_count;
         ++index) {
        UmiApplicationLaunchChoice choice;
        status = umi_application_launch_selection_at(
            launch_selection, index, &choice);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Apply this branch only when its contract condition is satisfied. */
        if (!choice.eligible) continue;
        gtk_box_append(GTK_BOX(desk->application_choices),
                       make_launch_choice(desk, &choice));
    }
    status = refresh_selection_controls(desk);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    strip = umi_desk_runtime_application_strip(desk->runtime);
    clear_box(desk->application_strip);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < snapshot.strip.item_count; ++index) {
        UmiDesktopApplicationStripItem item;
        status = umi_desktop_application_strip_at(strip, index, &item);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        gtk_box_append(GTK_BOX(desk->application_strip),
                       make_application_button(desk, &item));
        /* Apply this operation only while the related capability or state is available. */
        if (item.active) {
            gtk_label_set_text(GTK_LABEL(desk->workbench_title),
                               item.display_name);
            gtk_label_set_text(
                GTK_LABEL(desk->workbench_description),
                item.running
                    ? "Application is active. Framework panels and linked "
                      "contexts can be composed into this workbench."
                    : "Application is selected but not running.");
        }
    }

    clear_box(desk->layout_strip);
    /* Apply this branch only when its contract condition is satisfied. */
    if (snapshot.has_shell) {
        UmiDesktopShellModel *shell = umi_desk_runtime_shell(desk->runtime);
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < snapshot.shell.tab_count; ++index) {
            UmiDesktopShellTab tab;
            status = umi_desktop_shell_model_tab_at(shell, index, &tab);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;
            gtk_box_append(GTK_BOX(desk->layout_strip),
                           make_layout_button(desk, &tab));
        }
        {
            GtkWidget *new_layout = gtk_button_new_with_label("+");
            gtk_widget_add_css_class(new_layout, "flat");
            gtk_widget_set_tooltip_text(
                new_layout, "Create a new semantic layout");
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
    set_status(desk, status_text);
    return UMI_STATUS_OK;
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
