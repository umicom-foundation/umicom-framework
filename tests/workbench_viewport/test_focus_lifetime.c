/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_viewport/test_focus_lifetime.c
 *
 * PURPOSE:
 *   Verify panel navigation survives closure, layout changes and recursive native focus callbacks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout/viewport_gtk4.h"
#include "../../examples/workbench_viewport/practice_layout.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)

typedef struct Probe Probe;
typedef struct FocusPanel { GtkWidget parent; Probe *probe; size_t index; } FocusPanel;
typedef struct FocusPanelClass { GtkWidgetClass parent; } FocusPanelClass;
struct Probe {
    GtkWidget *view, *panels[4];
    GtkWindow *window;
    GtkEventController *keys;
    size_t created, calls[4], destroyed;
    bool armed, fired, accept, recursiveHandled;
    const char *action;
    UmiStatus mutation;
};
G_DEFINE_TYPE(FocusPanel, focus_panel, GTK_TYPE_WIDGET)

static gboolean SendKey(Probe *probe, bool reverse)
{
    gboolean handled = FALSE;
    g_signal_emit_by_name(probe->keys, "key-pressed", GDK_KEY_F6, 0U,
        reverse ? GDK_SHIFT_MASK : (GdkModifierType)0, &handled);
    return handled;
}

/* A real GTK leaf implements the focus virtual method so the test can close
 * or edit its owner at the exact handoff point. There is no replacement
 * viewport implementation, synthetic geometry result or broker connection. */
static gboolean Focus(GtkWidget *widget, GtkDirectionType direction)
{
    (void)direction;
    FocusPanel *panel = (FocusPanel *)widget;
    Probe *probe = panel->probe;
    if (!probe->armed) return FALSE;
    ++probe->calls[panel->index];
    if (!probe->fired) {
        probe->fired = true;
        if (!strcmp(probe->action, "close")) gtk_window_destroy(probe->window);
        else if (!strcmp(probe->action, "dispose")) {
            gtk_window_set_child(probe->window, NULL);
            g_object_run_dispose(G_OBJECT(probe->view));
        }
        else if (!strcmp(probe->action, "hide"))
            probe->mutation = UmiWorkbenchViewportWidgetSetVisible(probe->view, "status", false);
        else if (!strcmp(probe->action, "tab"))
            probe->mutation = UmiWorkbenchViewportWidgetSetActive(probe->view, "documents", 1);
        else if (!strcmp(probe->action, "recursive"))
            probe->recursiveHandled = SendKey(probe, false) != FALSE;
    }
    if (strcmp(probe->action, "normal")) return probe->accept ? TRUE : FALSE;
    if (!probe->accept && panel->index == 0) return FALSE;
    return gtk_widget_grab_focus(widget);
}

static void focus_panel_class_init(FocusPanelClass *klass)
{
    GTK_WIDGET_CLASS(klass)->focus = Focus;
}
static void focus_panel_init(FocusPanel *panel)
{
    gtk_widget_set_focusable(GTK_WIDGET(panel), TRUE);
    gtk_widget_set_size_request(GTK_WIDGET(panel), 80, 60);
}
static void Gone(gpointer data, GObject *object)
{
    (void)object;
    ++((Probe *)data)->destroyed;
}
static GtkWidget *Factory(const UmiWorkbenchLayoutNode *node, void *data)
{
    (void)node;
    Probe *probe = data;
    if (probe->created == 4) return NULL;
    FocusPanel *panel = g_object_new(focus_panel_get_type(), NULL);
    panel->probe = probe;
    panel->index = probe->created;
    probe->panels[probe->created++] = GTK_WIDGET(panel);
    g_object_weak_ref(G_OBJECT(panel), Gone, probe);
    return GTK_WIDGET(panel);
}
static GtkEventController *Keys(GtkWidget *view)
{
    GListModel *controllers = gtk_widget_observe_controllers(view);
    GtkEventController *keys = NULL;
    for (guint i = 0; i < g_list_model_get_n_items(controllers); ++i) {
        GObject *controller = g_list_model_get_item(controllers, i);
        if (GTK_IS_EVENT_CONTROLLER_KEY(controller)) { keys = GTK_EVENT_CONTROLLER(controller); break; }
        g_object_unref(controller);
    }
    g_object_unref(controllers);
    return keys;
}
static void Drain(void)
{
    for (unsigned i = 0; i < 64U && g_main_context_pending(NULL); ++i)
        (void)g_main_context_iteration(NULL, FALSE);
}
static int Run(const char *name)
{
    Probe probe = {.action = "normal", .accept = true};
    bool reverse = !strcmp(name, "reverse");
    bool hidden = !strcmp(name, "hidden");
    bool detached = !strcmp(name, "detached");
    if (!strcmp(name, "skip")) probe.accept = false;
    else if (!strncmp(name, "close-", 6)) { probe.action = "close"; probe.accept = !strcmp(name, "close-accept"); }
    else if (!strncmp(name, "dispose-", 8)) { probe.action = "dispose"; probe.accept = !strcmp(name, "dispose-accept"); }
    else if (!strcmp(name, "hide-callback")) { probe.action = "hide"; probe.accept = false; }
    else if (!strcmp(name, "tab-callback")) { probe.action = "tab"; probe.accept = false; }
    else if (!strcmp(name, "recursive")) probe.action = "recursive";
    else if (strcmp(name, "forward") && !reverse && !hidden && !detached && strcmp(name, "locked")) return 2;

    UmiWorkbenchLayoutDocument *document = malloc(sizeof *document);
    CHECK(document);
    UmiViewportLessonCreate(document);
    if (hidden) document->nodes[2].visibility = UMI_WORKBENCH_LAYOUT_VISIBILITY_HIDDEN;
    if (!strcmp(name, "locked")) document->flags |= UMI_WORKBENCH_LAYOUT_DOCUMENT_LOCKED;
    UmiStatus status = UmiWorkbenchViewportWidgetCreate(document, Factory, &probe, &probe.view, NULL);
    free(document);
    CHECK(status == UMI_STATUS_OK && probe.created == 4);
    g_object_ref_sink(probe.view);
    probe.window = GTK_WINDOW(gtk_window_new());
    g_object_ref(probe.window);
    gtk_window_set_default_size(probe.window, 1000, 600);
    gtk_window_set_child(probe.window, probe.view);
    gtk_window_present(probe.window);
    Drain();
    gtk_widget_allocate(probe.view, 1000, 600, -1, NULL);
    gtk_window_set_focus(probe.window, NULL);
    /* The fixture tests leaf acceptance, not the scroller fallback focus. */
    for (size_t i = 0; i < 4; ++i) {
        GtkWidget *parent = gtk_widget_get_parent(probe.panels[i]);
        while (parent && parent != probe.view) {
            gtk_widget_set_focusable(parent, FALSE);
            parent = gtk_widget_get_parent(parent);
        }
    }
    probe.keys = Keys(probe.view);
    CHECK(probe.keys);
    if (detached) gtk_window_set_child(probe.window, NULL);
    probe.armed = true;
    gboolean handled = SendKey(&probe, reverse);
    probe.armed = false;
    size_t total = 0;
    for (size_t i = 0; i < 4; ++i) total += probe.calls[i];
    if (detached) CHECK(!handled && total == 0);
    else if (!strcmp(probe.action, "normal")) {
        CHECK(handled);
        size_t expected = reverse ? 3U : (hidden || !probe.accept) ? 1U : 0U;
        CHECK(probe.calls[expected] == 1);
        CHECK(gtk_root_get_focus(GTK_ROOT(probe.window)) == probe.panels[expected]);
        CHECK(total == (!probe.accept ? 2U : 1U));
    } else if (!strcmp(probe.action, "recursive")) {
        CHECK(handled && !probe.recursiveHandled && total == 1);
    } else {
        CHECK(!handled && total == 1);
        if (!strcmp(probe.action, "hide") || !strcmp(probe.action, "tab")) {
            CHECK(probe.mutation == UMI_STATUS_OK);
            /* A fresh allocation makes the next independent key command valid. */
            gtk_widget_allocate(probe.view, 1000, 600, -1, NULL);
            probe.action = "normal"; probe.accept = true; probe.armed = true;
            CHECK(SendKey(&probe, false));
            probe.armed = false;
        }
    }
    gtk_window_destroy(probe.window);
    g_object_unref(probe.keys);
    g_object_unref(probe.view);
    g_object_unref(probe.window);
    Drain();
    CHECK(probe.destroyed == 4);
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (!gtk_init_check()) { puts("NOT RUN: no GTK display is available."); return 77; }
    /* A stale widget warning is a failure, not evidence of successful focus. */
    g_log_set_always_fatal((GLogLevelFlags)(G_LOG_FATAL_MASK | G_LOG_LEVEL_CRITICAL));
    return Run(argv[1]);
}
