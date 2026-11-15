/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/live_build_output/test_presenter_reset.c
 *
 * PURPOSE:
 *   Verify output reset clears accepted and paused text while preserving callback ownership.
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
static int empty_text(GtkWidget *text)
{
    GtkTextIter start, end;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text));
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    char *value = gtk_text_buffer_get_text(buffer, &start, &end, TRUE);
    int empty = value[0] == '\0'; g_free(value); return empty;
}
typedef struct ResetProbe {
    GtkWidget *owner;
    UmiOutputViewSnapshot *snapshot;
    const char *name;
    unsigned calls;
    UmiStatus status;
} ResetProbe;
static void changed(GtkTextBuffer *buffer, gpointer data)
{
    (void)buffer;
    ResetProbe *probe = data;
    if (probe->calls++ != 0U) return;
    if (strcmp(probe->name, "release") == 0) g_clear_object(&probe->owner);
    else if (strcmp(probe->name, "nested-reset") == 0)
        probe->status = UmiOutputViewGtk4Reset(probe->owner);
    else if (strcmp(probe->name, "nested-update") == 0)
        probe->status = UmiOutputViewGtk4Update(probe->owner, probe->snapshot);
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *cases[] = { "paused", "following", "restart-identity", "nested-reset", "nested-update", "release", "invalid", "retained" }; int known = 0;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
        if (strcmp(argv[1], cases[i]) == 0) known = 1;
    CHECK(known);
    (void)g_setenv("GTK_A11Y", "test", TRUE);
    if (!gtk_init_check()) return 77;
    ResetProbe probe = {.name = argv[1], .status = UMI_STATUS_OK};
    probe.owner = UmiOutputViewGtk4Create("reset.output", "Ready for this context.");
    CHECK(probe.owner != NULL); g_object_ref_sink(probe.owner);
    gpointer weak = probe.owner; g_object_add_weak_pointer(G_OBJECT(probe.owner), &weak);
    GtkWidget *text = find(probe.owner, "reset.output.text");
    GtkWidget *label = find(probe.owner, "reset.output.status");
    GtkWidget *follow = find(probe.owner, "reset.output.follow");
    GtkWidget *refresh = find(probe.owner, "reset.output.refresh");
    CHECK(text && label && follow && refresh);
    g_object_ref(text); g_object_ref(label); g_object_ref(follow); g_object_ref(refresh);
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text)); g_object_ref(buffer);
    probe.snapshot = g_new0(UmiOutputViewSnapshot, 1);
    probe.snapshot->operation_id = 7U; probe.snapshot->revision = 9U;
    strcpy(probe.snapshot->context, "Previous workspace");
    strcpy(probe.snapshot->status, "Previous report");
    strcpy(probe.snapshot->bytes, "private old output");
    probe.snapshot->length = probe.snapshot->total_bytes = strlen(probe.snapshot->bytes);
    CHECK(UmiOutputViewGtk4Update(probe.owner, probe.snapshot) == UMI_STATUS_OK);
    int following = strcmp(probe.name, "following") == 0;
    if (!following) gtk_check_button_set_active(GTK_CHECK_BUTTON(follow), FALSE);
    if (strcmp(probe.name, "paused") == 0) {
        probe.snapshot->revision++;
        strcpy(probe.snapshot->bytes, "newer old output");
        probe.snapshot->length = probe.snapshot->total_bytes = strlen(probe.snapshot->bytes);
        CHECK(UmiOutputViewGtk4Update(probe.owner, probe.snapshot) == UMI_STATUS_OK);
    }
    if (strcmp(probe.name, "invalid") == 0) {
        GtkWidget *other = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0); g_object_ref_sink(other);
        CHECK(UmiOutputViewGtk4Reset(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiOutputViewGtk4Reset(other) == UMI_STATUS_INVALID_ARGUMENT);
        g_object_unref(other);
        CHECK(!empty_text(text));
    } else {
        gulong handler = g_signal_connect(buffer, "changed", G_CALLBACK(changed), &probe);
        CHECK(UmiOutputViewGtk4Reset(probe.owner) == UMI_STATUS_OK);
        CHECK(probe.calls > 0U);
        g_signal_handler_disconnect(buffer, handler);
        CHECK(empty_text(text));
        CHECK(gtk_check_button_get_active(GTK_CHECK_BUTTON(follow)) == following);
        CHECK(strcmp(gtk_label_get_text(GTK_LABEL(label)), "Ready for this context.") == 0);
        if (strcmp(probe.name, "release") == 0) CHECK(weak == NULL && probe.owner == NULL);
        else {
            if (strncmp(probe.name, "nested-", 7U) == 0) CHECK(probe.status == UMI_STATUS_BUSY);
            g_signal_emit_by_name(refresh, "clicked"); CHECK(empty_text(text));
            if (strcmp(probe.name, "restart-identity") == 0) {
                probe.snapshot->operation_id = 1U; probe.snapshot->revision = 1U;
                strcpy(probe.snapshot->bytes, "current");
                probe.snapshot->length = probe.snapshot->total_bytes = 7U;
                CHECK(UmiOutputViewGtk4Update(probe.owner, probe.snapshot) == UMI_STATUS_OK);
                CHECK(empty_text(text)); /* Follow remains deliberately paused. */
                g_signal_emit_by_name(refresh, "clicked"); CHECK(!empty_text(text));
            }
        }
    }
    g_clear_object(&probe.owner); CHECK(weak == NULL);
    if (strcmp(probe.name, "retained") == 0 || strcmp(probe.name, "release") == 0) {
        g_signal_emit_by_name(refresh, "clicked");
        g_signal_emit_by_name(follow, "toggled");
    }
    g_clear_object(&buffer); g_clear_object(&text); g_clear_object(&label);
    g_clear_object(&follow); g_clear_object(&refresh);
    g_free(probe.snapshot);
    return 0;
}
