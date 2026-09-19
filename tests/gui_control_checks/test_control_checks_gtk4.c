/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/gui_control_checks/test_control_checks_gtk4.c
 *
 * PURPOSE:
 *   Exercise real GTK controls, delayed dialogs, private fields and the shared
 *   recorder's control-map export. These are toolkit tests, not OS input tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/interaction_recording.h"
#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); failed = 1; goto finish; } } while (0)

static void Pump(void)
{
    gint64 until = g_get_monotonic_time() + 120000;
    while (g_get_monotonic_time() < until) {
        for (unsigned i = 0U; i < 16U && g_main_context_pending(NULL); ++i)
            (void)g_main_context_iteration(NULL, FALSE);
        g_usleep(1000U);
    }
}
static UmiStatus Action(UmiUiAutomationDriver *driver, const char *target,
    UmiUiAutomationOperation operation, const char *value, uint32_t timeout,
    UmiUiAutomationObservation *observation)
{
    UmiUiAutomationStep step = {0}; char message[UMI_UI_AUTOMATION_MESSAGE_CAPACITY];
    step.operation = operation; step.timeout_ms = timeout;
    (void)snprintf(step.step_id, sizeof(step.step_id), "notes-step");
    (void)snprintf(step.target_id, sizeof(step.target_id), "%s", target);
    (void)snprintf(step.value, sizeof(step.value), "%s", value);
    return driver->perform(driver->context, &step, observation, message, sizeof(message));
}
static GtkWidget *Tagged(GtkWidget *widget, const char *id)
{
    if (umi_gtk4_automation_tag_widget(widget, id) != UMI_STATUS_OK) return NULL;
    return widget;
}
static GtkWidget *Find(GtkWidget *widget, const char *id)
{
    const char *tag = g_object_get_data(G_OBJECT(widget), "umicom-automation-id");
    if (tag != NULL && strcmp(tag, id) == 0) return widget;
    for (GtkWidget *child = gtk_widget_get_first_child(widget); child != NULL; child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *match = Find(child, id); if (match != NULL) return match;
    }
    return NULL;
}
typedef struct Delayed { GtkWidget *box; GtkWidget *old; GtkWidget *created; } Delayed;
static gboolean AddDelayed(gpointer data)
{
    Delayed *delayed = data;
    if (delayed->old != NULL) { gtk_box_remove(GTK_BOX(delayed->box), delayed->old); delayed->old = NULL; }
    delayed->created = Tagged(gtk_entry_new(), "notes.later");
    gtk_editable_set_text(GTK_EDITABLE(delayed->created), "New control");
    gtk_box_append(GTK_BOX(delayed->box), delayed->created);
    return G_SOURCE_REMOVE;
}
static gboolean StayPending(gpointer data) { (void)data; return G_SOURCE_CONTINUE; }
static void CountClick(GtkButton *button, gpointer data) { (void)button; ++*(unsigned *)data; }
static GtkWindow *FindRecorder(GtkWindow *owner)
{
    GListModel *windows = gtk_window_get_toplevels();
    for (guint i = 0U; i < g_list_model_get_n_items(windows); ++i) {
        GtkWindow *window = g_list_model_get_item(windows, i);
        if (gtk_window_get_transient_for(window) == owner && Find(GTK_WIDGET(window), "recording.window") != NULL)
            return window;
        g_object_unref(window);
    }
    return NULL;
}
static unsigned CountMaps(const char *directory)
{
    GDir *dir = g_dir_open(directory, 0U, NULL); const char *name; unsigned count = 0U;
    if (dir == NULL) return 0U;
    while ((name = g_dir_read_name(dir)) != NULL) {
        char *path = g_build_filename(directory, name, NULL);
        if (g_file_test(path, G_FILE_TEST_IS_DIR)) count += CountMaps(path);
        else if (g_str_has_prefix(name, "controls-") && (g_str_has_suffix(name, ".json") || g_str_has_suffix(name, ".html"))) ++count;
        g_free(path);
    }
    g_dir_close(dir); return count;
}
int main(int argc, char **argv)
{
    GtkWindow *window = NULL, *other = NULL, *recorder = NULL;
    GtkWidget *box = NULL, *entry = NULL, *child, *container, *root = NULL;
    UmiGtk4AutomationDriver *native = NULL, *isolated = NULL;
    UmiUiAutomationDriver driver = {0}, second = {0};
    UmiUiAutomationObservation observation = {0};
    UmiUiControlInventory *inventory = NULL; UmiUiControlInventorySummary summary;
    char *directory = NULL; guint idle = 0U, timer = 0U;
    Delayed delayed = {0}; unsigned calls = 0U; int failed = 0;
    const char *name;
    if (argc != 2) return 2;
    if (!gtk_init_check()) return 77;
    name = argv[1];
    directory = g_dir_make_tmp("umicom-control-check-XXXXXX", NULL); CHECK(directory != NULL);
    window = GTK_WINDOW(gtk_window_new()); gtk_window_set_default_size(window, 640, 400);
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8); gtk_window_set_child(window, box);
    gtk_box_append(GTK_BOX(box), gtk_label_new("Umicom Notes control checks"));
    entry = Tagged(gtk_entry_new(), "notes.title"); CHECK(entry != NULL); gtk_box_append(GTK_BOX(box), entry);
    gtk_window_present(window); Pump();
    CHECK(umi_gtk4_automation_driver_create(window, &native) == UMI_STATUS_OK);
    driver = umi_gtk4_automation_driver_interface(native);
    if (strcmp(name, "text-view") == 0 || strcmp(name, "read-only") == 0) {
        child = Tagged(gtk_text_view_new(), "notes.editor"); CHECK(child != NULL);
        gtk_box_append(GTK_BOX(box), child); Pump();
        CHECK(Action(&driver, "notes.editor", UMI_UI_AUTOMATION_TYPE_TEXT, "Umicom Notes\nSaved: 2 \xe2\x82\xac", 0U, &observation) == UMI_STATUS_OK);
        CHECK(Action(&driver, "notes.editor", UMI_UI_AUTOMATION_ASSERT_TEXT, "Umicom Notes\nSaved: 2 \xe2\x82\xac", 0U, &observation) == UMI_STATUS_OK);
        if (strcmp(name, "read-only") == 0) {
            gtk_text_view_set_editable(GTK_TEXT_VIEW(child), FALSE);
            CHECK(Action(&driver, "notes.editor", UMI_UI_AUTOMATION_TYPE_TEXT, "Replacement", 0U, &observation) == UMI_STATUS_PERMISSION_DENIED);
            CHECK(Action(&driver, "notes.editor", UMI_UI_AUTOMATION_ASSERT_TEXT, "Umicom Notes\nSaved: 2 \xe2\x82\xac", 0U, &observation) == UMI_STATUS_OK);
            gtk_editable_set_editable(GTK_EDITABLE(entry), FALSE);
            CHECK(Action(&driver, "notes.title", UMI_UI_AUTOMATION_TYPE_TEXT, "Replacement", 0U, &observation) == UMI_STATUS_PERMISSION_DENIED);
            CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(entry)), "") == 0);
        }
    } else if (strcmp(name, "inherited-disabled") == 0) {
        container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        child = Tagged(gtk_check_button_new_with_label("Saved"), "notes.saved"); CHECK(child != NULL);
        gtk_box_append(GTK_BOX(container), child); gtk_box_append(GTK_BOX(box), container); Pump();
        gtk_widget_set_sensitive(container, FALSE);
        CHECK(gtk_widget_get_sensitive(child));
        CHECK(Action(&driver, "notes.saved", UMI_UI_AUTOMATION_ASSERT_ENABLED, "false", 0U, &observation) == UMI_STATUS_OK);
        CHECK(Action(&driver, "notes.saved", UMI_UI_AUTOMATION_TOGGLE, "true", 0U, &observation) == UMI_STATUS_UNAVAILABLE);
        CHECK(!gtk_check_button_get_active(GTK_CHECK_BUTTON(child)));
    } else if (strcmp(name, "effective-visibility") == 0) {
        gtk_widget_set_visible(box, FALSE); Pump();
        CHECK(Action(&driver, "notes.title", UMI_UI_AUTOMATION_ASSERT_VISIBLE, "false", 0U, &observation) == UMI_STATUS_OK);
        CHECK(Action(&driver, "notes.title", UMI_UI_AUTOMATION_WAIT_VISIBLE, "", 15U, &observation) == UMI_STATUS_TIMEOUT);
        gtk_widget_set_visible(box, TRUE);
        CHECK(Action(&driver, "notes.title", UMI_UI_AUTOMATION_WAIT_VISIBLE, "", 1000U, &observation) == UMI_STATUS_OK);
    } else if (strcmp(name, "delayed-control") == 0 || strcmp(name, "removed-control") == 0) {
        delayed.box = box;
        if (strcmp(name, "removed-control") == 0) {
            delayed.old = Tagged(gtk_entry_new(), "notes.later"); CHECK(delayed.old != NULL);
            gtk_widget_set_sensitive(delayed.old, FALSE); gtk_box_append(GTK_BOX(box), delayed.old);
        }
        timer = g_timeout_add(10U, AddDelayed, &delayed);
        CHECK(Action(&driver, "notes.later", UMI_UI_AUTOMATION_WAIT_ENABLED, "", 1000U, &observation) == UMI_STATUS_OK);
        timer = 0U;
        CHECK(Action(&driver, "notes.later", UMI_UI_AUTOMATION_ASSERT_TEXT, "New control", 0U, &observation) == UMI_STATUS_OK);
    } else if (strcmp(name, "recurring-idle") == 0) {
        gint64 started = g_get_monotonic_time(); idle = g_idle_add(StayPending, NULL);
        CHECK(Action(&driver, "notes.absent", UMI_UI_AUTOMATION_WAIT_VISIBLE, "", 20U, &observation) == UMI_STATUS_TIMEOUT);
        CHECK(g_get_monotonic_time() - started < 2000000);
    } else if (strcmp(name, "duplicate-id") == 0) {
        for (unsigned i = 0U; i < 2U; ++i) {
            child = Tagged(gtk_button_new_with_label("Save"), "notes.save"); CHECK(child != NULL);
            g_signal_connect(child, "clicked", G_CALLBACK(CountClick), &calls);
            gtk_box_append(GTK_BOX(box), child);
        }
        CHECK(Action(&driver, "notes.save", UMI_UI_AUTOMATION_CLICK, "", 0U, &observation) == UMI_STATUS_ALREADY_EXISTS);
        Pump(); CHECK(calls == 0U);
    } else if (strcmp(name, "isolated-root") == 0) {
        root = g_object_ref_sink(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
        CHECK(umi_gtk4_automation_driver_create(root, &isolated) == UMI_STATUS_OK);
        second = umi_gtk4_automation_driver_interface(isolated);
        CHECK(Action(&second, "notes.title", UMI_UI_AUTOMATION_TYPE_TEXT, "Wrong host", 0U, &observation) == UMI_STATUS_NOT_FOUND);
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(entry)), "") == 0);
    } else if (strcmp(name, "check-button") == 0) {
        child = Tagged(gtk_check_button_new_with_label("Saved"), "notes.saved"); CHECK(child != NULL);
        gtk_box_append(GTK_BOX(box), child);
        CHECK(Action(&driver, "notes.saved", UMI_UI_AUTOMATION_TOGGLE, "true", 0U, &observation) == UMI_STATUS_OK);
        CHECK(Action(&driver, "notes.saved", UMI_UI_AUTOMATION_ASSERT_SELECTED, "true", 0U, &observation) == UMI_STATUS_OK);
        CHECK(Action(&driver, "notes.saved", UMI_UI_AUTOMATION_TOGGLE, "false", 0U, &observation) == UMI_STATUS_OK);
        CHECK(!gtk_check_button_get_active(GTK_CHECK_BUTTON(child)));
    } else if (strcmp(name, "private-text") == 0) {
        child = Tagged(gtk_password_entry_new(), "notes.password"); CHECK(child != NULL);
        gtk_editable_set_text(GTK_EDITABLE(child), "test-password-not-a-credential");
        gtk_box_append(GTK_BOX(box), child);
        CHECK(Action(&driver, "notes.password", UMI_UI_AUTOMATION_CAPTURE_EVIDENCE, "", 0U, &observation) == UMI_STATUS_OK);
        CHECK(observation.text[0] == '\0');
        CHECK(Action(&driver, "notes.password", UMI_UI_AUTOMATION_ASSERT_TEXT, "test-password-not-a-credential", 0U, &observation) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(UmiGtk4RecordingSetPrivate(box, 1) == UMI_STATUS_OK);
        CHECK(Action(&driver, "notes.title", UMI_UI_AUTOMATION_TYPE_TEXT, "Replacement", 0U, &observation) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(entry)), "") == 0);
    } else if (strcmp(name, "inventory") == 0) {
        child = Tagged(gtk_button_new_with_label("Duplicate"), "notes.title"); CHECK(child != NULL);
        gtk_box_append(GTK_BOX(box), child);
        gtk_box_append(GTK_BOX(box), gtk_button_new_with_label("No identifier"));
        gtk_box_append(GTK_BOX(box), gtk_password_entry_new());
        other = GTK_WINDOW(gtk_window_new()); gtk_window_set_child(other, Tagged(gtk_button_new(), "foreign.only"));
        CHECK(UmiGtk4ControlInventoryCapture(window, 1024U, &inventory) == UMI_STATUS_OK);
        CHECK(UmiUiControlInventorySummarise(inventory, &summary) == UMI_STATUS_OK);
        CHECK(summary.windows == 1U && summary.ambiguousTargets == 2U && summary.privateControls == 1U);
        CHECK(summary.unaddressableInteractive >= 1U && summary.inheritedTargets >= 1U);
        UmiUiControlInventoryDestroy(inventory); inventory = NULL;
        CHECK(UmiGtk4ControlInventoryCapture(window, 1U, &inventory) == UMI_STATUS_CAPACITY_EXCEEDED && inventory == NULL);
    } else if (strcmp(name, "inspection-export") == 0) {
        CHECK(UmiGtk4RecordingPanelShowAt(window, "umicom.notes", directory) == UMI_STATUS_OK); Pump();
        recorder = FindRecorder(window); CHECK(recorder != NULL);
        child = Find(GTK_WIDGET(recorder), "recording.inspect-controls"); CHECK(child != NULL);
        /* Activate the actual production button. This does not simulate OS input. */
        g_signal_emit_by_name(child, "clicked"); Pump();
        child = Find(GTK_WIDGET(recorder), "recording.status"); CHECK(GTK_IS_LABEL(child));
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(child)), "Control map saved") != NULL);
        CHECK(CountMaps(directory) >= 2U);
        child = Find(GTK_WIDGET(recorder), "recording.inspect-controls"); g_object_ref(child);
        gtk_window_destroy(window); window = NULL; Pump();
        g_signal_emit_by_name(child, "clicked"); g_object_unref(child);
    } else if (strcmp(name, "selection-boundary") == 0) {
        const char *choices[] = {"Notes", "Archive", NULL};
        child = Tagged(gtk_drop_down_new_from_strings(choices), "notes.folder"); CHECK(child != NULL);
        gtk_box_append(GTK_BOX(box), child);
        CHECK(Action(&driver, "notes.folder", UMI_UI_AUTOMATION_SELECT, "-1", 0U, &observation) == UMI_STATUS_PARSE_ERROR);
        CHECK(Action(&driver, "notes.folder", UMI_UI_AUTOMATION_SELECT, "2", 0U, &observation) == UMI_STATUS_NOT_FOUND);
        CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(child)) == 0U);
        CHECK(Action(&driver, "notes.folder", UMI_UI_AUTOMATION_SELECT, "1", 0U, &observation) == UMI_STATUS_OK);
        CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(child)) == 1U);
    } else CHECK(0);
finish:
    if (timer != 0U && delayed.created == NULL) g_source_remove(timer);
    if (idle != 0U) g_source_remove(idle);
    if (failed && window != NULL && directory != NULL) {
        char *image = g_build_filename(directory, "failure.png", NULL);
        UmiStatus captured = UmiGtk4RecordingCapturePng(window, image);
        fprintf(stderr, "Failure screenshot: %s (%s)\n", image, umi_status_text(captured)); g_free(image);
    }
    UmiUiControlInventoryDestroy(inventory);
    umi_gtk4_automation_driver_destroy(isolated); umi_gtk4_automation_driver_destroy(native);
    if (root != NULL) g_object_unref(root);
    if (other != NULL) gtk_window_destroy(other);
    if (window != NULL) gtk_window_destroy(window);
    if (recorder != NULL) g_object_unref(recorder);
    printf("Fixture output: %s\n", directory != NULL ? directory : "not created"); g_free(directory);
    return failed;
}
