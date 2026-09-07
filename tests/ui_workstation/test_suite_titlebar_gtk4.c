/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_suite_titlebar_gtk4.c
 * Purpose: Verify existing Suite identity moves to a native titlebar unchanged.
 * Author: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/suite_layout/gtk4_workstation.h"
#include "umicom/application/suite_layout/suite_layout.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(expression) do { \
    if (!(expression)) { \
        (void)fprintf(stderr, "Line %d: %s\n", __LINE__, #expression); \
        failed = 1; goto cleanup; \
    } \
} while (0)

typedef struct SuiteTitlebarFixture {
    size_t created;
    size_t released;
} SuiteTitlebarFixture;

/* Provider lifetime must not change merely because application chrome moves. */
static void panel_released(gpointer data, GObject *object)
{
    SuiteTitlebarFixture *fixture = data;
    (void)object;
    ++fixture->released;
}

/* Use real GTK bodies without any product, process, storage or network service. */
static GtkWidget *create_panel(const UmiUiWorkspaceWindow *window, void *data)
{
    SuiteTitlebarFixture *fixture = data;
    GtkWidget *body = gtk_label_new(window->title);
    ++fixture->created;
    g_object_weak_ref(G_OBJECT(body), panel_released, fixture);
    return body;
}

/* Stable tags observe production controls without reaching into private state. */
static GtkWidget *find_tag(GtkWidget *root, const char *id)
{
    GtkWidget *child;
    const char *tag;
    if (root == NULL) return NULL;
    tag = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (tag != NULL && strcmp(tag, id) == 0) return root;
    for (child = gtk_widget_get_first_child(root); child != NULL;
         child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = find_tag(child, id);
        if (found != NULL) return found;
    }
    return NULL;
}

/* These fixtures must never realize/present a window, including detached ones. */
static int all_windows_unpresented(void)
{
    GListModel *windows = gtk_window_get_toplevels();
    for (guint index = 0U; index < g_list_model_get_n_items(windows); ++index) {
        GtkWidget *window = g_list_model_get_item(windows, index);
        int visible = gtk_widget_get_visible(window) || gtk_widget_get_realized(window);
        g_object_unref(window);
        if (visible) return 0;
    }
    return 1;
}

/* Exercise real standalone creation, rejected binding, successful transfer and
 * both teardown orders for every different product identity in the test. */
static int check_suite(const char *application_id, const char *title, int window_first)
{
    SuiteTitlebarFixture fixture = {0};
    UmiApplicationSuiteGtk4WorkstationConfig config = {
        application_id, title, "Safe test mode", create_panel, &fixture
    };
    UmiApplicationSuiteGtk4Workstation *workstation = NULL;
    UmiUiWorkspaceLayout *projected = calloc(1U, sizeof(*projected));
    UmiUiAutomationDriver driver = {0};
    UmiUiAutomationStep step = {0};
    UmiUiAutomationObservation observation = {0};
    char message[UMI_UI_AUTOMATION_MESSAGE_CAPACITY];
    UmiApplicationSuiteGtk4WorkstationSnapshot *before = calloc(1U, sizeof(*before));
    UmiApplicationSuiteGtk4WorkstationSnapshot *after = calloc(1U, sizeof(*after));
    GtkWindow *window = NULL;
    GtkWindow *rejected_window = NULL;
    GtkWidget *root = NULL;
    GtkWidget *new_window = NULL;
    GtkWidget *catalogue;
    GtkWidget *identity;
    GtkWidget *old_parent;
    GtkWidget *search;
    GtkWidget *bar;
    GtkWidget *context;
    size_t provider_count;
    guint clicked_signal = 0U;
    int failed = 0;
    CHECK(before != NULL && after != NULL && projected != NULL);
    /* Stop before construction if a future preset would create/present a
     * detached native window. Detection after construction would be too late. */
    CHECK(umi_application_suite_layout_project_default(application_id, projected) == UMI_STATUS_OK);
    for (size_t index = 0U; index < projected->window_count; ++index)
        CHECK(!projected->windows[index].visible || !projected->windows[index].floating);
    CHECK(umi_application_suite_gtk4_workstation_create(&config, &workstation) == UMI_STATUS_OK);
    root = g_object_ref(umi_application_suite_gtk4_workstation_widget(workstation));
    *before = umi_application_suite_gtk4_workstation_snapshot(workstation);
    CHECK(!before->checkpoint_storage_bound && strcmp(before->identity.title, title) == 0);
    CHECK(strcmp(before->identity.mode_badge, "Safe test mode") == 0);
    identity = find_tag(root, "workstation.identity.title");
    catalogue = find_tag(root, "workstation.application.catalogue");
    search = find_tag(root, "workstation.application.search");
    CHECK(GTK_IS_LABEL(identity) && GTK_IS_MENU_BUTTON(catalogue) && GTK_IS_EDITABLE(search));
    old_parent = gtk_widget_get_parent(catalogue);
    new_window = find_tag(root, "workstation.application.new-window");
    if (new_window != NULL) g_object_ref(new_window);
    CHECK(GTK_IS_BUTTON(new_window));
    gtk_editable_set_text(GTK_EDITABLE(search), "retained catalogue search");
    clicked_signal = g_signal_lookup("clicked", GTK_TYPE_BUTTON);
    CHECK(clicked_signal != 0U && g_signal_has_handler_pending(new_window, clicked_signal, 0U, TRUE));

    rejected_window = GTK_WINDOW(gtk_window_new());
    g_object_ref_sink(rejected_window);
    gtk_window_set_titlebar(rejected_window, gtk_header_bar_new());
    CHECK(umi_application_suite_gtk4_workstation_bind_window(workstation, rejected_window)
        == UMI_STATUS_INVALID_STATE);
    CHECK(gtk_widget_get_parent(catalogue) == old_parent);
    CHECK(find_tag(root, "workstation.identity.title") == identity);
    CHECK(umi_application_suite_gtk4_workstation_set_mode_badge(workstation, "Safe test mode")
        == UMI_STATUS_OK);

    window = GTK_WINDOW(gtk_window_new());
    g_object_ref_sink(window);
    gtk_window_set_title(window, title);
    gtk_window_set_child(window, root);
    provider_count = fixture.created;
    CHECK(umi_application_suite_gtk4_workstation_bind_window(workstation, window) == UMI_STATUS_OK);
    bar = gtk_window_get_titlebar(window);
    CHECK(GTK_IS_HEADER_BAR(bar));
    CHECK(gtk_window_get_child(window) == root && fixture.created == provider_count);
    CHECK(find_tag(root, "workstation.identity.title") == NULL);
    CHECK(find_tag(bar, "workstation.identity.title") == identity);
    CHECK(find_tag(bar, "workstation.application.catalogue") == catalogue);
    CHECK(find_tag(bar, "workstation.application.new-window") == new_window);
    CHECK(find_tag(bar, "workstation.application.search") == search);
    CHECK(gtk_widget_get_visible(catalogue) && gtk_widget_get_visible(new_window));
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(search)), "retained catalogue search") == 0);
    CHECK(gtk_widget_has_css_class(bar, "umicom-appearance-scope"));
    /* The original UAT driver reaches the reparented catalogue. Typing into
     * its real search control is safe; no launch or popup signal is emitted. */
    CHECK(umi_application_suite_gtk4_workstation_automation(workstation, &driver) == UMI_STATUS_OK);
    (void)g_strlcpy(step.target_id, "workstation.application.catalogue", sizeof(step.target_id));
    step.operation = UMI_UI_AUTOMATION_CAPTURE_EVIDENCE;
    CHECK(driver.perform(driver.context, &step, &observation, message, sizeof(message)) == UMI_STATUS_OK);
    CHECK(strcmp(observation.role_name, "GtkMenuButton") == 0 && observation.visible);
    (void)g_strlcpy(step.target_id, "workstation.application.search", sizeof(step.target_id));
    (void)g_strlcpy(step.value, "Catalogue query through original driver", sizeof(step.value));
    step.operation = UMI_UI_AUTOMATION_TYPE_TEXT;
    CHECK(driver.perform(driver.context, &step, &observation, message, sizeof(message)) == UMI_STATUS_OK);
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(search)), step.value) == 0);
    CHECK(umi_application_suite_gtk4_workstation_bind_window(workstation, window) == UMI_STATUS_OK);
    CHECK(umi_application_suite_gtk4_workstation_bind_window(workstation, rejected_window)
        == UMI_STATUS_INVALID_STATE);
    *after = umi_application_suite_gtk4_workstation_snapshot(workstation);
    CHECK(strcmp(before->identity.application_id, after->identity.application_id) == 0);
    CHECK(strcmp(before->identity.title, after->identity.title) == 0);
    CHECK(strcmp(before->identity.mode_badge, after->identity.mode_badge) == 0);
    CHECK(strcmp(before->identity.icon_resource, after->identity.icon_resource) == 0);
    CHECK(strcmp(before->appearance.active_profile_id, after->appearance.active_profile_id) == 0);
    CHECK(strcmp(before->active_layout_id, after->active_layout_id) == 0);
    CHECK(before->rendered_panel_count == after->rendered_panel_count);
    context = find_tag(bar, "workstation.window-titlebar.context");
    CHECK(GTK_IS_LABEL(context));
    CHECK(strcmp(gtk_label_get_text(GTK_LABEL(context)), after->active_layout_name) == 0);
    CHECK(strstr(gtk_window_get_title(window), after->active_layout_name) != NULL);
    CHECK(all_windows_unpresented());

    if (window_first) {
        gtk_window_destroy(window);
        g_object_unref(window);
        window = NULL;
        *after = umi_application_suite_gtk4_workstation_snapshot(workstation);
        CHECK(strcmp(after->identity.title, title) == 0);
    }
    umi_application_suite_gtk4_workstation_destroy(workstation);
    workstation = NULL;
    /* Never emit clicked: a teardown regression must not start a real product. */
    CHECK(!g_signal_has_handler_pending(new_window, clicked_signal, 0U, TRUE));
    CHECK(gtk_widget_get_first_child(root) == NULL);
    CHECK(fixture.created == fixture.released);
    CHECK(all_windows_unpresented());
cleanup:
    umi_application_suite_gtk4_workstation_destroy(workstation);
    if (window != NULL) { gtk_window_destroy(window); g_object_unref(window); }
    if (rejected_window != NULL) { gtk_window_destroy(rejected_window); g_object_unref(rejected_window); }
    g_clear_object(&new_window);
    g_clear_object(&root);
    free(projected);
    free(after);
    free(before);
    return failed;
}

/* Explicit scopes cannot target another window just because their primary
 * content root has not been parented yet. Native windows are never retained
 * by the additional scope, and nested trees remain safely observable. */
static int check_scope_isolation(void)
{
    UmiGtk4AutomationDriver *native = NULL;
    UmiUiAutomationDriver driver = {0};
    UmiUiAutomationStep step = {0};
    UmiUiAutomationObservation observation = {0};
    GtkWidget *root = g_object_ref_sink(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    GtkWidget *scope = g_object_ref_sink(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    GtkWidget *entry = gtk_entry_new();
    GtkWidget *foreign = gtk_entry_new();
    GtkWindow *other = GTK_WINDOW(gtk_window_new());
    GtkWindow *own = GTK_WINDOW(gtk_window_new());
    char message[UMI_UI_AUTOMATION_MESSAGE_CAPACITY];
    int failed = 0;
    g_object_ref_sink(other);
    g_object_ref_sink(own);
    gtk_box_append(GTK_BOX(scope), entry);
    gtk_window_set_child(other, foreign);
    gtk_window_set_child(own, scope);
    CHECK(umi_gtk4_automation_tag_widget(entry, "scope.same") == UMI_STATUS_OK);
    CHECK(umi_gtk4_automation_tag_widget(foreign, "scope.same") == UMI_STATUS_OK);
    gtk_editable_set_text(GTK_EDITABLE(foreign), "Other application's draft");
    CHECK(umi_gtk4_automation_driver_create(root, &native) == UMI_STATUS_OK);
    CHECK(umi_gtk4_automation_driver_add_observed_scope(native, other) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_gtk4_automation_driver_add_observed_scope(native, root) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(umi_gtk4_automation_driver_add_observed_scope(native, scope) == UMI_STATUS_OK);
    CHECK(umi_gtk4_automation_driver_add_observed_scope(native, scope) == UMI_STATUS_ALREADY_EXISTS);
    driver = umi_gtk4_automation_driver_interface(native);
    (void)g_strlcpy(step.target_id, "scope.same", sizeof(step.target_id));
    (void)g_strlcpy(step.value, "Only our observed scope", sizeof(step.value));
    step.operation = UMI_UI_AUTOMATION_TYPE_TEXT;
    CHECK(driver.perform(driver.context, &step, &observation, message, sizeof(message)) == UMI_STATUS_OK);
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(entry)), step.value) == 0);
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(foreign)), "Other application's draft") == 0);
    CHECK(umi_gtk4_automation_tag_widget(foreign, "scope.foreign-only") == UMI_STATUS_OK);
    (void)g_strlcpy(step.target_id, "scope.foreign-only", sizeof(step.target_id));
    CHECK(driver.perform(driver.context, &step, &observation, message, sizeof(message)) == UMI_STATUS_NOT_FOUND);
    /* Moving the scope beneath content must not require another owned scope. */
    gtk_window_set_child(own, NULL);
    gtk_box_append(GTK_BOX(root), scope);
    (void)g_strlcpy(step.target_id, "scope.same", sizeof(step.target_id));
    step.operation = UMI_UI_AUTOMATION_CAPTURE_EVIDENCE;
    CHECK(driver.perform(driver.context, &step, &observation, message, sizeof(message)) == UMI_STATUS_OK);
    CHECK(strcmp(observation.text, "Only our observed scope") == 0);
    CHECK(all_windows_unpresented());
cleanup:
    umi_gtk4_automation_driver_destroy(native);
    gtk_window_destroy(other);
    g_object_unref(other);
    gtk_window_destroy(own);
    g_object_unref(own);
    g_object_unref(root);
    g_object_unref(scope);
    return failed;
}

/* Cover dedicated products and a representative shared preview with no launch
 * signals, storage opt-in, realization or presentation in this test. */
int main(void)
{
    static const struct { const char *id; const char *title; } products[] = {
        {"org.umicom.trader", "Umicom Trader"},
        {"org.umicom.bank", "Umicom Bank"},
        {"org.umicom.tms", "Umicom TMS"},
        {"org.umicom.music-studio", "Umicom Music Studio"},
        {"org.umicom.cad", "Umicom CAD"}
    };
    (void)g_setenv("GTK_A11Y", "test", TRUE);
    if (!gtk_init_check()) return 77;
    if (check_scope_isolation()) return 1;
    for (size_t index = 0U; index < G_N_ELEMENTS(products); ++index)
        if (check_suite(products[index].id, products[index].title, (int)(index % 2U))) return 1;
    return 0;
}
