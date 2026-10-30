/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_focus_cycle_gtk4.c
 *
 * PURPOSE:
 *   Exercise native panel navigation, draft retention and focus callback retirement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/workstation/focus_cycle.h"
#include <stdio.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); failed = 1; goto done; } } while (0)

static void Settle(void)
{
    gint64 deadline = g_get_monotonic_time() + G_TIME_SPAN_SECOND / 3;
    while (g_get_monotonic_time() < deadline) {
        for (unsigned i = 0U; i < 32U && g_main_context_pending(NULL); ++i)
            (void)g_main_context_iteration(NULL, FALSE);
        g_usleep(1000U);
    }
}

static gboolean Inside(GtkWindow *window, GtkWidget *region)
{
    GtkWidget *focus = gtk_window_get_focus(window);
    return focus != NULL && (focus == region || gtk_widget_is_ancestor(focus, region));
}

static void Clicked(GtkButton *button, gpointer context)
{
    (void)button;
    ++*(unsigned *)context;
}

typedef struct Observer {
    const char *mode;
    GtkWidget *workspace;
    GtkWidget *target;
    GtkWidget *outside;
    gboolean fired;
    gboolean nested_result;
} Observer;

static void FocusChanged(GObject *object, GParamSpec *property, gpointer context)
{
    (void)property;
    Observer *observer = context;
    GtkWindow *window = GTK_WINDOW(object);
    if (observer->fired || !Inside(window, observer->target)) return;
    observer->fired = TRUE;
    if (strcmp(observer->mode, "reentry") == 0)
        observer->nested_result = UmiGtk4FocusCycleAdvance(observer->workspace, FALSE);
    else if (strcmp(observer->mode, "redirect") == 0)
        (void)gtk_widget_grab_focus(observer->outside);
    else if (strcmp(observer->mode, "detach") == 0)
        gtk_box_remove(GTK_BOX(observer->workspace), observer->target);
    else if (strcmp(observer->mode, "retire") == 0)
        gtk_window_destroy(window);
}

int main(int argc, char **argv)
{
    static const char *const cases[] = {"forward", "reverse", "wrap", "hidden",
        "disabled", "notebook", "single", "no-target", "nested", "reentry",
        "redirect", "detach", "retire", "modifiers", "controller", "paned"};
    if (argc != 2) return 2;
    gboolean known = FALSE;
    for (size_t i = 0U; i < G_N_ELEMENTS(cases); ++i)
        if (strcmp(argv[1], cases[i]) == 0) known = TRUE;
    if (!known) return 2;
    if (!gtk_init_check()) return 77;
    const char *mode = argv[1];
    int failed = 0;
    unsigned clicks = 0U;
    GtkWidget *window = g_object_ref_sink(gtk_window_new());
    GtkWidget *workspace = g_object_ref_sink(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12));
    GtkWidget *outer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *outside = gtk_entry_new();
    GtkWidget *regions[3] = {0};
    GtkWidget *entries[3] = {0};
    GtkEventController *keys = NULL;
    gulong observer_id = 0UL;
    Observer observer = {mode, workspace, NULL, outside, FALSE, TRUE};
    gtk_window_set_child(GTK_WINDOW(window), outer);
    gtk_box_append(GTK_BOX(outer), workspace);
    gtk_box_append(GTK_BOX(outer), outside);
    gtk_window_set_default_size(GTK_WINDOW(window), 900, 300);
    for (size_t i = 0U; i < 3U; ++i) {
        regions[i] = g_object_ref_sink(gtk_box_new(GTK_ORIENTATION_VERTICAL, 6));
        entries[i] = g_object_ref_sink(gtk_entry_new());
        gtk_editable_set_text(GTK_EDITABLE(entries[i]), "Unsaved local draft");
        gtk_box_append(GTK_BOX(regions[i]), entries[i]);
        GtkWidget *button = gtk_button_new_with_label("Apply");
        gtk_box_append(GTK_BOX(regions[i]), button);
        g_signal_connect(button, "clicked", G_CALLBACK(Clicked), &clicks);
        gtk_box_append(GTK_BOX(workspace), regions[i]);
        UmiGtk4FocusCycleMarkRegion(regions[i], TRUE);
    }
    UmiGtk4FocusCycleInstall(workspace);
    UmiGtk4FocusCycleInstall(workspace);
    GListModel *controllers = gtk_widget_observe_controllers(workspace);
    unsigned key_count = 0U;
    for (guint i = 0U; i < g_list_model_get_n_items(controllers); ++i) {
        GObject *item = g_list_model_get_item(controllers, i);
        if (GTK_IS_EVENT_CONTROLLER_KEY(item)) {
            ++key_count;
            g_clear_object(&keys);
            keys = GTK_EVENT_CONTROLLER(g_object_ref(item));
        }
        g_object_unref(item);
    }
    g_object_unref(controllers);
    CHECK(key_count == 1U && keys != NULL);
    CHECK(gtk_event_controller_get_propagation_phase(keys) == GTK_PHASE_BUBBLE);
    gtk_window_present(GTK_WINDOW(window));
    Settle();
    CHECK(gtk_widget_get_mapped(workspace));
    CHECK(gtk_widget_grab_focus(entries[0]));
    CHECK(Inside(GTK_WINDOW(window), regions[0]));
    if (strcmp(mode, "hidden") == 0 || strcmp(mode, "disabled") == 0) {
        if (strcmp(mode, "hidden") == 0) gtk_widget_set_visible(regions[1], FALSE);
        else gtk_widget_set_sensitive(regions[1], FALSE);
        CHECK(UmiGtk4FocusCycleAdvance(workspace, FALSE));
        CHECK(Inside(GTK_WINDOW(window), regions[2]));
    } else if (strcmp(mode, "reverse") == 0 || strcmp(mode, "wrap") == 0) {
        if (strcmp(mode, "wrap") == 0) CHECK(gtk_widget_grab_focus(entries[2]));
        CHECK(UmiGtk4FocusCycleAdvance(workspace, strcmp(mode, "reverse") == 0));
        CHECK(Inside(GTK_WINDOW(window), regions[strcmp(mode, "reverse") == 0 ? 2 : 0]));
    } else if (strcmp(mode, "single") == 0 || strcmp(mode, "no-target") == 0) {
        UmiGtk4FocusCycleMarkRegion(regions[1], FALSE);
        UmiGtk4FocusCycleMarkRegion(regions[2], FALSE);
        if (strcmp(mode, "no-target") == 0) UmiGtk4FocusCycleMarkRegion(regions[0], FALSE);
        GtkWidget *before = gtk_window_get_focus(GTK_WINDOW(window));
        CHECK(!UmiGtk4FocusCycleAdvance(workspace, FALSE));
        CHECK(gtk_window_get_focus(GTK_WINDOW(window)) == before);
    } else if (strcmp(mode, "nested") == 0) {
        UmiGtk4FocusCycleMarkRegion(regions[1], FALSE);
        UmiGtk4FocusCycleInstall(regions[1]);
        UmiGtk4FocusCycleMarkRegion(entries[1], TRUE);
        CHECK(UmiGtk4FocusCycleAdvance(workspace, FALSE));
        CHECK(Inside(GTK_WINDOW(window), regions[2]));
        CHECK(UmiGtk4FocusCycleAdvance(regions[1], FALSE));
        CHECK(Inside(GTK_WINDOW(window), regions[1]));
    } else if (strcmp(mode, "paned") == 0) {
        GtkWidget *split = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
        GtkWidget *right = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
        for (size_t i = 0U; i < 3U; ++i)
            gtk_box_remove(GTK_BOX(workspace), regions[i]);
        gtk_paned_set_start_child(GTK_PANED(split), regions[0]);
        gtk_paned_set_end_child(GTK_PANED(split), right);
        gtk_paned_set_start_child(GTK_PANED(right), regions[1]);
        gtk_paned_set_end_child(GTK_PANED(right), regions[2]);
        gtk_box_append(GTK_BOX(workspace), split);
        UmiGtk4FocusCycleInstall(workspace);
        UmiGtk4FocusCycleInstall(workspace);
        Settle();
        CHECK(gtk_widget_grab_focus(entries[0]));
        gboolean handled = FALSE;
        g_signal_emit_by_name(split, "cycle-child-focus", FALSE, &handled);
        CHECK(handled && Inside(GTK_WINDOW(window), regions[1]));
        g_signal_emit_by_name(right, "cycle-child-focus", FALSE, &handled);
        CHECK(handled && Inside(GTK_WINDOW(window), regions[2]));
        g_signal_emit_by_name(right, "cycle-child-focus", TRUE, &handled);
        CHECK(handled && Inside(GTK_WINDOW(window), regions[1]));
    } else if (strcmp(mode, "notebook") == 0) {
        /* Only the selected notebook page participates. Marked descendants
         * of a region do not create extra panel stops. */
        GtkWidget *book = gtk_notebook_new();
        GtkWidget *hidden = gtk_entry_new();
        gtk_box_remove(GTK_BOX(workspace), regions[1]);
        gtk_box_remove(GTK_BOX(workspace), regions[2]);
        gtk_notebook_append_page(GTK_NOTEBOOK(book), regions[1], gtk_label_new("First"));
        gtk_notebook_append_page(GTK_NOTEBOOK(book), regions[2], gtk_label_new("Second"));
        gtk_box_append(GTK_BOX(regions[1]), hidden);
        UmiGtk4FocusCycleMarkRegion(hidden, TRUE);
        gtk_box_append(GTK_BOX(workspace), book);
        gtk_notebook_set_current_page(GTK_NOTEBOOK(book), 1);
        Settle();
        CHECK(gtk_widget_grab_focus(entries[0]));
        CHECK(UmiGtk4FocusCycleAdvance(workspace, FALSE));
        CHECK(Inside(GTK_WINDOW(window), regions[2]));
        CHECK(!gtk_widget_get_mapped(regions[1]));
    } else if (strcmp(mode, "modifiers") == 0 || strcmp(mode, "controller") == 0) {
        gboolean handled = FALSE;
        if (strcmp(mode, "modifiers") == 0) {
            const GdkModifierType modifiers[] = {GDK_CONTROL_MASK, GDK_ALT_MASK,
                GDK_SUPER_MASK, GDK_HYPER_MASK, GDK_META_MASK};
            for (size_t i = 0U; i < G_N_ELEMENTS(modifiers); ++i) {
                g_signal_emit_by_name(keys, "key-pressed", GDK_KEY_F6, 0U, modifiers[i], &handled);
                CHECK(!handled && Inside(GTK_WINDOW(window), regions[0]));
            }
            g_signal_emit_by_name(keys, "key-pressed", GDK_KEY_F7, 0U, (GdkModifierType)0, &handled);
            CHECK(!handled);
        }
        g_signal_emit_by_name(keys, "key-pressed", GDK_KEY_F6, 0U, (GdkModifierType)0, &handled);
        CHECK(handled && Inside(GTK_WINDOW(window), regions[1]));
        g_signal_emit_by_name(keys, "key-pressed", GDK_KEY_F6, 0U, GDK_SHIFT_MASK, &handled);
        CHECK(handled && Inside(GTK_WINDOW(window), regions[0]));
    } else if (strcmp(mode, "forward") == 0) {
        CHECK(UmiGtk4FocusCycleAdvance(workspace, FALSE));
        CHECK(Inside(GTK_WINDOW(window), regions[1]));
        CHECK(UmiGtk4FocusCycleAdvance(workspace, FALSE));
        CHECK(Inside(GTK_WINDOW(window), regions[2]));
    } else {
        observer.target = regions[1];
        observer_id = g_signal_connect(window, "notify::focus-widget", G_CALLBACK(FocusChanged), &observer);
        gboolean moved = UmiGtk4FocusCycleAdvance(workspace, FALSE);
        CHECK(observer.fired);
        if (strcmp(mode, "reentry") == 0) {
            CHECK(moved && !observer.nested_result && Inside(GTK_WINDOW(window), regions[1]));
        } else {
            CHECK(!moved);
            if (strcmp(mode, "redirect") == 0) CHECK(Inside(GTK_WINDOW(window), outside));
            if (strcmp(mode, "detach") == 0) CHECK(gtk_widget_get_parent(regions[1]) == NULL);
            if (strcmp(mode, "retire") == 0) CHECK(!gtk_widget_get_mapped(workspace));
        }
    }
    CHECK(clicks == 0U);
    for (size_t i = 0U; i < 3U; ++i)
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(entries[i])), "Unsaved local draft") == 0);
done:
    if (observer_id != 0UL) g_signal_handler_disconnect(window, observer_id);
    gtk_window_destroy(GTK_WINDOW(window));
    g_clear_object(&keys);
    for (size_t i = 0U; i < 3U; ++i) { g_clear_object(&regions[i]); g_clear_object(&entries[i]); }
    g_object_unref(workspace);
    g_object_unref(window);
    return failed;
}
