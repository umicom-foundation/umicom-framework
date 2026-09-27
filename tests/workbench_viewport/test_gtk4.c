/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Run against the real GTK library and an actual display. Exit 77 is a skip,
 * never a graphical pass. No database, file write or business service is used.
 */
#include "umicom/workbench_layout/viewport_gtk4.h"
#include "../../examples/workbench_viewport/practice_layout.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); return 1; } } while (0)
#define OK(expression) CHECK((expression) == UMI_STATUS_OK)

typedef struct Tracker {
    size_t calls, destroyed, failAt;
    GtkWidget *widgets[4];
    bool strong;
} Tracker;

static void Gone(gpointer data, GObject *object)
{
    (void)object;
    Tracker *tracker = data;
    ++tracker->destroyed;
}

static void RootGone(gpointer data, GObject *object)
{
    (void)object;
    bool *gone = data;
    *gone = true;
}

static GtkWidget *Factory(const UmiWorkbenchLayoutNode *node, void *data)
{
    (void)node;
    Tracker *tracker = data;
    size_t call = tracker->calls++;
    if (call == tracker->failAt || call >= 4) return NULL;
    GtkWidget *widget = gtk_text_view_new();
    tracker->widgets[call] = widget;
    g_object_weak_ref(G_OBJECT(widget), Gone, tracker);
    if (tracker->strong) g_object_ref_sink(widget);
    return widget;
}

static GtkWidget *FindToggle(GtkWidget *widget)
{
    if (GTK_IS_TOGGLE_BUTTON(widget)) return widget;
    for (GtkWidget *child = gtk_widget_get_first_child(widget); child;
        child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = FindToggle(child);
        if (found) return found;
    }
    return NULL;
}

static bool TextIs(GtkWidget *widget, const char *expected)
{
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    char *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
    bool equal = !strcmp(text, expected);
    g_free(text);
    return equal;
}

static int Run(const char *name, UmiWorkbenchLayoutDocument *document)
{
    Tracker tracker = {.failAt = (size_t)-1};
    GtkWidget *view = NULL;
    if (!strcmp(name, "invalid")) {
        document->nodes[1].split_ratio = 0;
        CHECK(UmiWorkbenchViewportWidgetCreate(document, Factory, &tracker, &view, NULL) != UMI_STATUS_OK);
        CHECK(!view && !tracker.calls && !tracker.destroyed);
        return 0;
    }
    if (!strcmp(name, "factory-failure")) {
        for (size_t failure = 0; failure < 4; ++failure) {
            tracker = (Tracker){.failAt = failure};
            CHECK(UmiWorkbenchViewportWidgetCreate(document, Factory, &tracker, &view, NULL) == UMI_STATUS_UNAVAILABLE);
            CHECK(!view && tracker.destroyed == failure);
        }
        return 0;
    }
    if (!strcmp(name, "gallery")) {
        GtkApplication *application = gtk_application_new("foundation.umicom.viewport.test", G_APPLICATION_NON_UNIQUE);
        GError *error = NULL;
        CHECK(g_application_register(G_APPLICATION(application), NULL, &error));
        GtkWindow *first = UmiWorkbenchViewportGallery(application);
        GtkWindow *second = UmiWorkbenchViewportGallery(application);
        CHECK(first && second && first != second);
        CHECK(gtk_window_get_child(first) && gtk_window_get_child(second));
        gtk_window_destroy(first);
        gtk_window_destroy(second);
        g_object_unref(application);
        return 0;
    }
    if (!strcmp(name, "locked")) document->flags |= UMI_WORKBENCH_LAYOUT_DOCUMENT_READ_ONLY;
    for (size_t iteration = 0; iteration < (!strcmp(name, "lifecycle") ? 128U : 1U); ++iteration) {
        tracker = (Tracker){.failAt = (size_t)-1, .strong = (iteration % 2U) != 0};
        OK(UmiWorkbenchViewportWidgetCreate(document, Factory, &tracker, &view, NULL));
        CHECK(view && tracker.calls == 4 && tracker.destroyed == 0);
        g_object_ref_sink(view);
        bool rootGone = false;
        g_object_weak_ref(G_OBJECT(view), RootGone, &rootGone);
        if (!strcmp(name, "retained-child")) {
            GtkWidget *button = FindToggle(view);
            CHECK(button);
            g_object_ref(button);
            g_object_unref(view);
            CHECK(rootGone && tracker.destroyed == 4);
            g_signal_emit_by_name(button, "clicked");
            g_object_unref(button);
            return 0;
        }
        if (!strcmp(name, "locked")) {
            CHECK(UmiWorkbenchViewportWidgetSetSplit(view, "columns", 0.4) == UMI_STATUS_PERMISSION_DENIED);
            CHECK(UmiWorkbenchViewportWidgetSetActive(view, "documents", 1) == UMI_STATUS_PERMISSION_DENIED);
            CHECK(UmiWorkbenchViewportWidgetSetVisible(view, "navigation", false) == UMI_STATUS_PERMISSION_DENIED);
        } else if (!strcmp(name, "preview-preserves-text")) {
            UmiWorkbenchLayoutDocument *original = malloc(sizeof *original);
            CHECK(original);
            *original = *document;
            gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(tracker.widgets[1])), "Keep this note.", -1);
            OK(UmiWorkbenchViewportWidgetSetActive(view, "documents", 1));
            OK(UmiWorkbenchViewportWidgetSetSplit(view, "columns", 0.45));
            OK(UmiWorkbenchViewportWidgetSetVisible(view, "navigation", false));
            OK(UmiWorkbenchViewportWidgetSetActive(view, "documents", 0));
            CHECK(TextIs(tracker.widgets[1], "Keep this note.") && !tracker.destroyed);
            CHECK(!memcmp(original, document, sizeof *original));
            free(original);
        } else if (!strcmp(name, "two-instances")) {
            Tracker other = {.failAt = (size_t)-1};
            GtkWidget *second = NULL;
            OK(UmiWorkbenchViewportWidgetCreate(document, Factory, &other, &second, NULL));
            g_object_ref_sink(second);
            gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(tracker.widgets[1])), "First only", -1);
            CHECK(TextIs(other.widgets[1], ""));
            g_object_unref(second);
            CHECK(other.destroyed == 4 && tracker.destroyed == 0);
        } else if (strcmp(name, "lifecycle")) {
            fprintf(stderr, "Unknown graphical case: %s\n", name);
            g_object_unref(view);
            return 1;
        }
        g_object_unref(view);
        CHECK(rootGone && tracker.destroyed == 4);
    }
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (!gtk_init_check()) {
        puts("NOT RUN: no GTK display is available.");
        return 77;
    }
    UmiWorkbenchLayoutDocument *document = malloc(sizeof *document);
    if (!document) return 2;
    UmiViewportLessonCreate(document);
    int result = Run(argv[1], document);
    free(document);
    return result;
}
