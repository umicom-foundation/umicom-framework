/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/live_build_output/test_presenter_lifetime.c
 *
 * PURPOSE:
 *   Exercise shared output owner retirement and nested GTK publication with real signal callbacks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/output_view.h"
#include <stdio.h>
#include <string.h>
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); return 1; } } while (0)

static GtkWidget *find(GtkWidget *root, const char *tag)
{
    const char *name = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (name != NULL && strcmp(name, tag) == 0) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL;
        child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = find(child, tag);
        if (found != NULL) return found;
    }
    return NULL;
}

static char *text_of(GtkWidget *text)
{
    GtkTextIter start, end;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text));
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    return gtk_text_buffer_get_text(buffer, &start, &end, TRUE);
}

typedef struct Probe {
    GtkWidget *owner, *window, *refresh, *follow, *copy;
    UmiOutputViewSnapshot *nested;
    const char *name;
    unsigned calls;
    UmiStatus nested_status;
} Probe;

static void act(Probe *probe)
{
    if (probe->calls++ != 0U) return;
    if (strstr(probe->name, "-release") != NULL) g_clear_object(&probe->owner);
    else if (strcmp(probe->name, "window-close") == 0) {
        gtk_window_destroy(GTK_WINDOW(probe->window));
        /* Native removal and object finalization differ when the test retains
         * the GtkWindow. Release that last external container owner as well,
         * so the case actually exercises panel finalization during Update. */
        g_clear_object(&probe->window);
        probe->owner = NULL; /* The window held the transferred panel reference. */
    } else if (strstr(probe->name, "-nested") != NULL)
        probe->nested_status = UmiOutputViewGtk4Update(probe->owner, probe->nested);
    else if (strcmp(probe->name, "recursive-refresh") == 0)
        g_signal_emit_by_name(probe->refresh, "clicked");
    else if (strcmp(probe->name, "recursive-follow") == 0)
        g_signal_emit_by_name(probe->follow, "toggled");
    else if (strcmp(probe->name, "recursive-copy") == 0)
        g_signal_emit_by_name(probe->copy, "clicked");
}

static void changed(GtkTextBuffer *buffer, gpointer data)
{
    (void)buffer; act(data);
}
static void labelled(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object; (void)property; act(data);
}

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *cases[] = { "update-release", "status-release", "refresh-release", "follow-release", "window-close", "update-nested", "status-nested", "refresh-nested", "follow-nested", "recursive-refresh", "recursive-follow", "recursive-copy" };
    int known = 0;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
        if (strcmp(argv[1], cases[i]) == 0) known = 1;
    CHECK(known);
    (void)g_setenv("GTK_A11Y", "test", TRUE);
    if (!gtk_init_check()) return 77;
    Probe probe = {.name = argv[1], .nested_status = UMI_STATUS_OK};
    probe.owner = UmiOutputViewGtk4Create("lifetime.output", "Ready.");
    CHECK(probe.owner != NULL); g_object_ref_sink(probe.owner);
    gpointer weak = probe.owner;
    g_object_add_weak_pointer(G_OBJECT(probe.owner), &weak);
    GtkWidget *text = find(probe.owner, "lifetime.output.text");
    GtkWidget *label = find(probe.owner, "lifetime.output.status");
    probe.refresh = find(probe.owner, "lifetime.output.refresh");
    probe.follow = find(probe.owner, "lifetime.output.follow");
    probe.copy = find(probe.owner, "lifetime.output.copy");
    CHECK(text && label && probe.refresh && probe.follow && probe.copy);
    /* Retained children let the test observe completed text and disconnect
     * callbacks even after the panel's last external owner has retired. */
    g_object_ref(text); g_object_ref(label); g_object_ref(probe.refresh);
    g_object_ref(probe.follow); g_object_ref(probe.copy);
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text));
    g_object_ref(buffer);
    UmiOutputViewSnapshot *snapshot = g_new0(UmiOutputViewSnapshot, 1);
    snapshot->operation_id = 1U; snapshot->revision = 1U;
    strcpy(snapshot->context, "Code or build output");
    strcpy(snapshot->status, "Initial");
    strcpy(snapshot->bytes, "first"); snapshot->length = snapshot->total_bytes = 5U;
    CHECK(UmiOutputViewGtk4Update(probe.owner, snapshot) == UMI_STATUS_OK);

    int status_case = strncmp(probe.name, "status-", 7U) == 0;
    int refresh_case = strncmp(probe.name, "refresh-", 8U) == 0;
    int follow_case = strncmp(probe.name, "follow-", 7U) == 0;
    if (status_case || refresh_case || follow_case)
        gtk_check_button_set_active(GTK_CHECK_BUTTON(probe.follow), FALSE);
    snapshot->revision = 2U; strcpy(snapshot->status, "Current");
    strcpy(snapshot->bytes, "second"); snapshot->length = snapshot->total_bytes = 6U;
    if (refresh_case || follow_case)
        CHECK(UmiOutputViewGtk4Update(probe.owner, snapshot) == UMI_STATUS_OK);
    probe.nested = g_new0(UmiOutputViewSnapshot, 1);
    *probe.nested = *snapshot;
    probe.nested->revision = 3U; strcpy(probe.nested->status, "Nested");
    strcpy(probe.nested->bytes, "third"); probe.nested->length = probe.nested->total_bytes = 5U;
    if (strcmp(probe.name, "window-close") == 0) {
        probe.window = gtk_window_new(); g_object_ref_sink(probe.window);
        gtk_window_set_child(GTK_WINDOW(probe.window), probe.owner);
        g_object_unref(probe.owner); /* Window is now the sole external owner. */
    }
    gulong handler = status_case ?
        g_signal_connect(label, "notify::label", G_CALLBACK(labelled), &probe) :
        g_signal_connect(buffer, "changed", G_CALLBACK(changed), &probe);
    if (refresh_case) g_signal_emit_by_name(probe.refresh, "clicked");
    else if (follow_case) gtk_check_button_set_active(GTK_CHECK_BUTTON(probe.follow), TRUE);
    else CHECK(UmiOutputViewGtk4Update(probe.owner, snapshot) == UMI_STATUS_OK);
    CHECK(probe.calls > 0U);
    g_signal_handler_disconnect(status_case ? G_OBJECT(label) : G_OBJECT(buffer), handler);
    if (strstr(probe.name, "-release") != NULL || strcmp(probe.name, "window-close") == 0) {
        CHECK(probe.owner == NULL && weak == NULL);
        /* Orphan controls must not reacquire a retired panel or call a service. */
        g_signal_emit_by_name(probe.refresh, "clicked");
        g_signal_emit_by_name(probe.copy, "clicked");
        g_signal_emit_by_name(probe.follow, "toggled");
    } else {
        CHECK(weak != NULL);
        char *displayed = text_of(text);
        CHECK(strcmp(displayed, status_case ? "first" : "second") == 0); g_free(displayed);
        if (strstr(probe.name, "-nested") != NULL) {
            CHECK(probe.nested_status == UMI_STATUS_BUSY);
            CHECK(strstr(gtk_label_get_text(GTK_LABEL(label)), "Current") != NULL);
            CHECK(UmiOutputViewGtk4Update(probe.owner, probe.nested) == UMI_STATUS_OK);
            gtk_check_button_set_active(GTK_CHECK_BUTTON(probe.follow), TRUE);
            displayed = text_of(text);
            CHECK(strcmp(displayed, "third") == 0); g_free(displayed);
        }
        g_clear_object(&probe.owner);
        CHECK(weak == NULL);
    }
    g_clear_object(&probe.window); g_clear_object(&buffer);
    g_clear_object(&text); g_clear_object(&label);
    g_clear_object(&probe.refresh); g_clear_object(&probe.follow); g_clear_object(&probe.copy);
    g_free(snapshot); g_free(probe.nested);
    return 0;
}
