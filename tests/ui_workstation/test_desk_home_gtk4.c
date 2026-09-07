/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_desk_home_gtk4.c
 *
 * PURPOSE:
 *   Verify the actual Desk Home widgets, governed requests and refresh/teardown
 *   lifetimes with inert launcher callbacks and no presented application window.
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
#include <string.h>

#define CHECK(expression) do { if (!(expression)) { \
    (void)fprintf(stderr, "Line %d: %s\n", __LINE__, #expression); \
    failed = 1; goto cleanup; } } while (0)

typedef struct HomeFixture {
    UmiGtk4Desk *desk;
    size_t starts;
    size_t activations;
    bool destroy_on_start;
    char last_application[UMI_APPLICATION_RUNTIME_ID_CAPACITY];
} HomeFixture;

/* Record a governed start without creating any process, file or window. */
static UmiStatus fake_start(void *context, const UmiApplicationLaunchPlan *plan,
    uint64_t *out_token)
{
    HomeFixture *fixture = context;
    ++fixture->starts;
    (void)g_strlcpy(fixture->last_application, plan->application_id,
        sizeof(fixture->last_application));
    *out_token = 100U + fixture->starts;
    if (fixture->destroy_on_start) {
        umi_gtk4_desk_destroy(fixture->desk);
        fixture->desk = NULL;
    }
    return UMI_STATUS_OK;
}

/* Activation acknowledges the request only; it cannot focus external windows. */
static UmiStatus fake_activate(void *context, const char *application_id, uint64_t token)
{
    HomeFixture *fixture = context;
    (void)token;
    ++fixture->activations;
    (void)g_strlcpy(fixture->last_application, application_id, sizeof(fixture->last_application));
    return UMI_STATUS_OK;
}

/* Find stable automation IDs within actual native descendants. */
static GtkWidget *find_id(GtkWidget *widget, const char *id)
{
    const char *actual;
    if (widget == NULL) return NULL;
    actual = g_object_get_data(G_OBJECT(widget), "umicom-automation-id");
    if (g_strcmp0(actual, id) == 0) return widget;
    for (GtkWidget *child = gtk_widget_get_first_child(widget); child != NULL;
         child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = find_id(child, id);
        if (found != NULL) return found;
    }
    return NULL;
}

/* Find the old chooser's distinct checkboxes without confusing Home controls. */
static GtkWidget *find_choice(GtkWidget *widget, const char *id)
{
    if (widget == NULL) return NULL;
    if (GTK_IS_CHECK_BUTTON(widget) && g_strcmp0(
            g_object_get_data(G_OBJECT(widget), "umicom-application-id"), id) == 0)
        return widget;
    for (GtkWidget *child = gtk_widget_get_first_child(widget); child != NULL;
         child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = find_choice(child, id);
        if (found != NULL) return found;
    }
    return NULL;
}

/* Read user-visible failure feedback without depending on a private label ID. */
static bool has_label_text(GtkWidget *widget, const char *text)
{
    if (widget == NULL) return false;
    if (GTK_IS_LABEL(widget) && strstr(gtk_label_get_text(GTK_LABEL(widget)), text) != NULL)
        return true;
    for (GtkWidget *child = gtk_widget_get_first_child(widget); child != NULL;
         child = gtk_widget_get_next_sibling(child))
        if (has_label_text(child, text)) return true;
    return false;
}

/* Drain only ready sources with a bound, never run the user's application loop. */
static void drain_ready(void)
{
    for (size_t index = 0U; index < 128U && g_main_context_pending(NULL); ++index)
        (void)g_main_context_iteration(NULL, FALSE);
}

/* Register canonical products with explicit fixture presence, not disk discovery. */
static UmiStatus register_fixture(UmiDeskRuntime *runtime, const char *id,
    const char *name, bool installed, bool compatible, bool enabled)
{
    UmiApplicationRuntimeRegistration registration = {0};
    registration.structure_size = sizeof(registration);
    registration.application_id = id; registration.display_name = name;
    registration.executable_name = "fixture-app";
    registration.working_directory = "";
    registration.icon_resource_id = "umicom.icon.application.studio";
    registration.default_layout_id = "develop";
    registration.taskbar_group = "test";
    registration.family = UMI_APPLICATION_FAMILY_DEVELOPMENT;
    registration.maturity = UMI_APPLICATION_AVAILABLE;
    registration.entry_kind = UMI_APPLICATION_ENTRY_WORKBENCH;
    registration.installed = installed; registration.compatible = compatible;
    registration.enabled = enabled; registration.visible_when_unavailable = true;
    return umi_desk_runtime_upsert_application(runtime, &registration);
}

/* Exercise the real Framework Desk composition without presenting its window. */
int main(void)
{
    UmiDeskRuntime *runtime = NULL;
    UmiDeskRuntimeConfig config = umi_desk_runtime_config_default();
    UmiApplicationLauncherAdapter adapter = {0};
    UmiApplicationLaunchSelectionSnapshot selection;
    UmiApplicationLaunchDispatchReport *header_report = NULL;
    UmiGtk4WorkstationShellHeader *header = NULL;
    UmiGtk4WorkstationShellHeaderConfig header_config;
    HomeFixture fixture = {0};
    GtkApplication *application = NULL;
    GtkWidget *root = NULL;
    GtkWidget *search;
    GtkWidget *launch;
    GtkWidget *select;
    GtkWidget *chooser;
    GtkWidget *control;
    GtkWidget *held_launch = NULL;
    GtkWidget *held_search = NULL;
    GtkWidget *held_home = NULL;
    GError *error = NULL;
    int start;
    int end;
    int failed = 0;

    if (!gtk_init_check()) return 77;
    config.seed_framework_portfolio = false;
    config.launcher.executable_root = ".";
    config.launcher.executable_suffix = "";
    adapter.structure_size = sizeof(adapter);
    adapter.adapter_context = &fixture;
    adapter.start = fake_start; adapter.activate = fake_activate;
    CHECK(umi_desk_runtime_create(NULL, &config, &adapter, &runtime) == UMI_STATUS_OK);
    CHECK(register_fixture(runtime, "org.umicom.studio", "Studio test name", true, true, true) == UMI_STATUS_OK);
    CHECK(register_fixture(runtime, "org.umicom.trader", "Trader test name", false, true, true) == UMI_STATUS_OK);
    CHECK(register_fixture(runtime, "org.umicom.bank", "Bank test name", true, true, false) == UMI_STATUS_OK);
    CHECK(register_fixture(runtime, "org.umicom.tms", "TMS test name", true, false, true) == UMI_STATUS_OK);
    application = gtk_application_new("org.umicom.tests.deskhome", G_APPLICATION_NON_UNIQUE);
    CHECK(g_application_register(G_APPLICATION(application), NULL, &error));
    CHECK(umi_gtk4_desk_create(application, runtime, &fixture.desk) == UMI_STATUS_OK);
    root = g_object_ref(gtk_window_get_child(GTK_WINDOW(umi_gtk4_desk_native_window(fixture.desk))));
    CHECK(strcmp(umi_gtk4_desk_visible_page(fixture.desk), "home") == 0);
    CHECK(umi_gtk4_desk_show_home(NULL) == UMI_STATUS_INVALID_ARGUMENT && umi_gtk4_desk_visible_page(NULL) == NULL);
    search = find_id(root, "desk.home.search");
    launch = find_id(root, "desk.home.open.org.umicom.studio");
    select = find_id(root, "desk.home.select.org.umicom.studio");
    chooser = find_choice(root, "org.umicom.studio");
    CHECK(GTK_IS_SEARCH_ENTRY(search) && GTK_IS_BUTTON(launch) && GTK_IS_CHECK_BUTTON(select));
    CHECK(chooser != NULL && gtk_widget_get_sensitive(launch));
    control = find_id(root, "desk.home.open.org.umicom.trader");
    CHECK(control != NULL && !gtk_widget_get_sensitive(control));
    CHECK(strstr(gtk_widget_get_tooltip_text(control), "not available") != NULL);
    g_signal_emit_by_name(control, "clicked"); drain_ready();
    CHECK(fixture.starts == 0U);
    CHECK(!gtk_widget_get_sensitive(find_id(root, "desk.home.open.org.umicom.bank")));
    CHECK(!gtk_widget_get_sensitive(find_id(root, "desk.home.open.org.umicom.tms")));
    /* Presentation cannot offer a second start during startup or shutdown,
     * including a synthetic button signal that bypasses native input routing. */
    CHECK(umi_application_runtime_catalogue_set_state(umi_desk_runtime_applications(runtime),
        "org.umicom.studio", UMI_APPLICATION_RUNTIME_STARTING, "fixture starting") == UMI_STATUS_OK);
    CHECK(umi_gtk4_desk_refresh(fixture.desk) == UMI_STATUS_OK);
    CHECK(!gtk_widget_get_sensitive(launch));
    g_signal_emit_by_name(launch, "clicked"); drain_ready();
    CHECK(fixture.starts == 0U);
    CHECK(umi_application_runtime_catalogue_set_state(umi_desk_runtime_applications(runtime),
        "org.umicom.studio", UMI_APPLICATION_RUNTIME_STOPPING, "fixture stopping") == UMI_STATUS_OK);
    CHECK(umi_gtk4_desk_refresh(fixture.desk) == UMI_STATUS_OK);
    CHECK(!gtk_widget_get_sensitive(launch));
    CHECK(umi_application_runtime_catalogue_set_state(umi_desk_runtime_applications(runtime),
        "org.umicom.studio", UMI_APPLICATION_RUNTIME_STOPPED, "fixture stopped") == UMI_STATUS_OK);
    CHECK(umi_gtk4_desk_refresh(fixture.desk) == UMI_STATUS_OK);
    CHECK(gtk_widget_get_sensitive(launch));
    chooser = find_choice(root, "org.umicom.studio");
    /* Search is case-insensitive and does not invoke application requests. */
    gtk_editable_set_text(GTK_EDITABLE(search), "STUDIO");
    g_signal_emit_by_name(search, "search-changed");
    CHECK(gtk_widget_get_visible(gtk_widget_get_ancestor(launch, GTK_TYPE_FLOW_BOX_CHILD)));
    CHECK(!gtk_widget_get_visible(gtk_widget_get_ancestor(control, GTK_TYPE_FLOW_BOX_CHILD)));
    gtk_editable_select_region(GTK_EDITABLE(search), 1, 4);
    CHECK(umi_gtk4_desk_refresh(fixture.desk) == UMI_STATUS_OK);
    CHECK(umi_gtk4_desk_refresh(fixture.desk) == UMI_STATUS_OK);
    CHECK(find_id(root, "desk.home.search") == search && find_id(root, "desk.home.open.org.umicom.studio") == launch);
    CHECK(find_choice(root, "org.umicom.studio") == chooser);
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(search)), "STUDIO") == 0);
    CHECK(gtk_editable_get_selection_bounds(GTK_EDITABLE(search), &start, &end) && start == 1 && end == 4);
    CHECK(fixture.starts == 0U && fixture.activations == 0U);
    gtk_editable_set_text(GTK_EDITABLE(search), "no matching application");
    g_signal_emit_by_name(search, "search-changed");
    CHECK(gtk_widget_get_visible(find_id(root, "desk.home.empty")));
    gtk_editable_set_text(GTK_EDITABLE(search), "");
    g_signal_emit_by_name(search, "search-changed");
    CHECK(!gtk_widget_get_visible(find_id(root, "desk.home.empty")));
    /* Home selection is the same core selection used by the original chooser. */
    gtk_check_button_set_active(GTK_CHECK_BUTTON(select), TRUE);
    CHECK(umi_application_launch_selection_snapshot(umi_desk_runtime_launch_selection(runtime), &selection) == UMI_STATUS_OK);
    CHECK(selection.selected_count == 1U && fixture.starts == 0U);
    CHECK(umi_gtk4_desk_refresh(fixture.desk) == UMI_STATUS_OK);
    CHECK(gtk_check_button_get_active(GTK_CHECK_BUTTON(find_choice(root, "org.umicom.studio"))));
    control = find_id(root, "desk.home.selection");
    g_signal_emit_by_name(control, "clicked");
    CHECK(strcmp(umi_gtk4_desk_visible_page(fixture.desk), "applications") == 0);
    g_signal_emit_by_name(find_id(root, "desk.home"), "clicked");
    CHECK(strcmp(umi_gtk4_desk_visible_page(fixture.desk), "home") == 0);
    /* Click queues one governed request; repeated clicks cannot double-start. */
    g_signal_emit_by_name(launch, "clicked"); g_signal_emit_by_name(launch, "clicked");
    CHECK(fixture.starts == 0U);
    drain_ready();
    CHECK(fixture.starts == 1U && strcmp(fixture.last_application, "org.umicom.studio") == 0);
    CHECK(find_id(root, "desk.home.open.org.umicom.studio") == launch);
    CHECK(strcmp(gtk_button_get_label(GTK_BUTTON(launch)), "Activate") == 0);
    CHECK(umi_desk_runtime_set_application_presence(runtime, "org.umicom.studio", false, true, true) == UMI_STATUS_OK);
    CHECK(umi_gtk4_desk_refresh(fixture.desk) == UMI_STATUS_OK);
    CHECK(gtk_widget_get_sensitive(launch));
    g_signal_emit_by_name(launch, "clicked"); drain_ready();
    CHECK(fixture.starts == 1U && fixture.activations == 1U);
    /* The topmost header must not bypass Desk's missing-file policy or invoke
     * a direct executable/PATH launcher. Existing processes reuse activation. */
    header_report = g_try_new0(UmiApplicationLaunchDispatchReport, 1);
    CHECK(header_report != NULL);
    header_config = umi_gtk4_ws_shell_header_config_default("org.umicom.desktop", "Desk test");
    CHECK(umi_gtk4_ws_shell_header_create_managed(&header_config, &header) == UMI_STATUS_OK);
    CHECK(umi_gtk4_desk_bind_shell_header(NULL, header) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_gtk4_desk_bind_shell_header(fixture.desk, header) == UMI_STATUS_OK);
    CHECK(umi_gtk4_desk_bind_shell_header(fixture.desk, header) == UMI_STATUS_OK);
    CHECK(umi_gtk4_ws_shell_header_catalogue_set_selected(header, "org.umicom.trader", true) == UMI_STATUS_OK);
    CHECK(umi_gtk4_ws_shell_header_catalogue_dispatch(header, header_report) != UMI_STATUS_OK);
    CHECK(header_report->accepted_count == 0U && header_report->failed_count == 1U && fixture.starts == 1U);
    CHECK(umi_gtk4_ws_shell_header_catalogue_set_selected(header, "org.umicom.trader", false) == UMI_STATUS_OK);
    CHECK(umi_gtk4_ws_shell_header_catalogue_set_selected(header, "org.umicom.studio", true) == UMI_STATUS_OK);
    CHECK(umi_gtk4_ws_shell_header_catalogue_dispatch(header, header_report) == UMI_STATUS_OK);
    CHECK(header_report->accepted_count == 1U && fixture.activations == 2U && fixture.starts == 1U);
    control = find_id(umi_gtk4_ws_shell_header_widget(header), "workstation.application.new-window");
    CHECK(GTK_IS_BUTTON(control));
    g_signal_emit_by_name(control, "clicked");
    CHECK(has_label_text(umi_gtk4_ws_shell_header_widget(header), umi_status_text(UMI_STATUS_NOT_IMPLEMENTED)));
    CHECK(fixture.activations == 2U && fixture.starts == 1U);
    /* Missing-at-dispatch must be revalidated by the governed runtime. */
    CHECK(umi_desk_runtime_set_application_presence(runtime, "org.umicom.trader", true, true, true) == UMI_STATUS_OK);
    CHECK(umi_gtk4_desk_refresh(fixture.desk) == UMI_STATUS_OK);
    /* Discovery's explicit selection-retention policy must remain removable
     * in both Home and the original chooser if a selected executable vanishes. */
    CHECK(umi_application_launch_selection_set_retention(umi_desk_runtime_launch_selection(runtime), true) == UMI_STATUS_OK);
    control = find_id(root, "desk.home.select.org.umicom.trader");
    gtk_check_button_set_active(GTK_CHECK_BUTTON(control), TRUE);
    CHECK(umi_desk_runtime_set_application_presence(runtime, "org.umicom.trader", false, true, true) == UMI_STATUS_OK);
    CHECK(umi_gtk4_desk_refresh(fixture.desk) == UMI_STATUS_OK);
    CHECK(gtk_widget_get_sensitive(control) && gtk_check_button_get_active(GTK_CHECK_BUTTON(control)));
    chooser = find_choice(root, "org.umicom.trader");
    CHECK(GTK_IS_CHECK_BUTTON(chooser) && gtk_widget_get_sensitive(chooser));
    gtk_check_button_set_active(GTK_CHECK_BUTTON(control), FALSE);
    CHECK(!gtk_check_button_get_active(GTK_CHECK_BUTTON(chooser)));
    CHECK(umi_desk_runtime_set_application_presence(runtime, "org.umicom.trader", true, true, true) == UMI_STATUS_OK);
    CHECK(umi_gtk4_desk_refresh(fixture.desk) == UMI_STATUS_OK);
    control = find_id(root, "desk.home.open.org.umicom.trader");
    CHECK(gtk_widget_get_sensitive(control));
    g_signal_emit_by_name(control, "clicked");
    CHECK(umi_desk_runtime_set_application_presence(runtime, "org.umicom.trader", false, true, true) == UMI_STATUS_OK);
    drain_ready();
    CHECK(fixture.starts == 1U && !gtk_widget_get_sensitive(control));
    /* Retained controls and queued work become inert after explicit teardown. */
    held_launch = g_object_ref(launch); held_search = g_object_ref(search);
    held_home = g_object_ref(find_id(root, "desk.home"));
    g_signal_emit_by_name(launch, "clicked");
    umi_gtk4_desk_destroy(fixture.desk); fixture.desk = NULL;
    drain_ready();
    g_signal_emit_by_name(held_launch, "clicked");
    g_signal_emit_by_name(held_home, "clicked");
    gtk_editable_set_text(GTK_EDITABLE(held_search), "retained");
    g_signal_emit_by_name(held_search, "search-changed"); drain_ready();
    CHECK(fixture.starts == 1U && fixture.activations == 2U);
    CHECK(umi_gtk4_ws_shell_header_catalogue_set_selected(header, "org.umicom.studio", true) == UMI_STATUS_OK);
    CHECK(umi_gtk4_ws_shell_header_catalogue_dispatch(header, header_report) == UMI_STATUS_INVALID_STATE);
    CHECK(header_report->failed_count == 1U && fixture.activations == 2U);
    CHECK(umi_gtk4_ws_shell_header_catalogue_set_selected(header, "org.umicom.studio", false) == UMI_STATUS_OK);
    g_clear_object(&held_launch); g_clear_object(&held_home); g_clear_object(&held_search);
    g_clear_object(&root);
    /* A launch adapter can close Desk while its request is in flight. */
    CHECK(umi_desk_runtime_set_application_presence(runtime, "org.umicom.trader", true, true, true) == UMI_STATUS_OK);
    CHECK(umi_gtk4_desk_create(application, runtime, &fixture.desk) == UMI_STATUS_OK);
    root = g_object_ref(gtk_window_get_child(GTK_WINDOW(umi_gtk4_desk_native_window(fixture.desk))));
    fixture.destroy_on_start = true;
    control = find_id(root, "desk.home.open.org.umicom.trader");
    CHECK(control != NULL && gtk_widget_get_sensitive(control));
    g_signal_emit_by_name(control, "clicked"); drain_ready();
    CHECK(fixture.desk == NULL && fixture.starts == 2U);
    g_clear_object(&root);
    /* Rebinding an inert surviving header is safe. Destruction inside its own
     * governed callback must not leave a dangling context for the next batch. */
    CHECK(umi_desk_runtime_set_application_presence(runtime, "org.umicom.bank", true, true, true) == UMI_STATUS_OK);
    CHECK(umi_gtk4_desk_create(application, runtime, &fixture.desk) == UMI_STATUS_OK);
    CHECK(umi_gtk4_desk_bind_shell_header(fixture.desk, header) == UMI_STATUS_OK);
    CHECK(umi_gtk4_ws_shell_header_catalogue_set_selected(header, "org.umicom.bank", true) == UMI_STATUS_OK);
    CHECK(umi_gtk4_ws_shell_header_catalogue_dispatch(header, header_report) == UMI_STATUS_OK);
    CHECK(fixture.desk == NULL && fixture.starts == 3U && header_report->accepted_count == 1U);
    CHECK(umi_gtk4_ws_shell_header_catalogue_set_selected(header, "org.umicom.studio", true) == UMI_STATUS_OK);
    CHECK(umi_gtk4_ws_shell_header_catalogue_dispatch(header, header_report) == UMI_STATUS_INVALID_STATE);
    CHECK(header_report->failed_count == 1U && fixture.activations == 2U);

cleanup:
    umi_gtk4_ws_shell_header_destroy(header);
    g_free(header_report);
    umi_gtk4_desk_destroy(fixture.desk);
    if (held_launch != NULL) g_object_unref(held_launch);
    if (held_search != NULL) g_object_unref(held_search);
    if (held_home != NULL) g_object_unref(held_home);
    if (root != NULL) g_object_unref(root);
    umi_desk_runtime_destroy(runtime);
    if (application != NULL) g_object_unref(application);
    g_clear_error(&error);
    return failed;
}
