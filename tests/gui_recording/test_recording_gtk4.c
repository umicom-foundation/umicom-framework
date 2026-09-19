/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/gui_recording/test_recording_gtk4.c
 * PURPOSE: Exercise PNG output, private widgets, reports and callback lifetime.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/interaction_recording.h"
#include "umicom/ui/gtk4/automation.h"
#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
/* The external CTest timeout also catches a wedged GTK owning thread. */
static void Pump(void)
{
    gint64 until = g_get_monotonic_time() + 250000;
    while (g_get_monotonic_time() < until) {
        for (unsigned i = 0U; i < 32U && g_main_context_pending(NULL); ++i)
            (void)g_main_context_iteration(NULL, FALSE);
        g_usleep(1000U);
    }
}
static GtkWidget *Find(GtkWidget *root, const char *id)
{
    const char *tag = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (tag != NULL && strcmp(tag, id) == 0) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL; child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = Find(child, id); if (found != NULL) return found;
    }
    return NULL;
}
static GtkWindow *Recorder(void)
{
    GListModel *windows = gtk_window_get_toplevels();
    for (guint i = 0U; i < g_list_model_get_n_items(windows); ++i) {
        GtkWindow *w = g_list_model_get_item(windows, i);
        if (Find(GTK_WIDGET(w), "recording.window") != NULL) return w;
        g_object_unref(w);
    }
    return NULL;
}
int main(void)
{
    GtkWindow *window, *panel; GtkWidget *box, *password, *start, *note, *add, *export;
    char *directory, *image, *existing; GdkTexture *texture;
    if (!gtk_init_check()) return 77;
    directory = g_dir_make_tmp("umicom-recording-test-XXXXXX", NULL); CHECK(directory != NULL);
    image = g_build_filename(directory, "window.png", NULL);
    window = GTK_WINDOW(gtk_window_new()); gtk_window_set_default_size(window, 500, 260);
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8); gtk_window_set_child(window, box);
    gtk_box_append(GTK_BOX(box), gtk_label_new("Umicom Notes: record a save problem"));
    password = gtk_password_entry_new(); gtk_box_append(GTK_BOX(box), password);
    gtk_widget_set_visible(password, FALSE); gtk_window_present(window); Pump();
    CHECK(UmiGtk4RecordingCapturePng(window, image) == UMI_STATUS_OK);
    texture = gdk_texture_new_from_filename(image, NULL); CHECK(texture != NULL);
    CHECK(gdk_texture_get_width(texture) > 0 && gdk_texture_get_height(texture) > 0); g_object_unref(texture);
    CHECK(UmiGtk4RecordingCapturePng(window, image) == UMI_STATUS_ALREADY_EXISTS);
    gtk_widget_set_visible(password, TRUE); Pump();
    CHECK(UmiGtk4RecordingContainsPrivate(window));
    CHECK(UmiGtk4RecordingCapturePng(window, image) == UMI_STATUS_PERMISSION_DENIED);
    gtk_widget_set_visible(password, FALSE);
    CHECK(UmiGtk4RecordingSetPrivate(box, 1) == UMI_STATUS_OK);
    CHECK(UmiGtk4RecordingCapturePng(window, image) == UMI_STATUS_PERMISSION_DENIED);
    CHECK(UmiGtk4RecordingContainsPrivate(gtk_widget_get_first_child(box)));
    CHECK(UmiGtk4RecordingCapturePng(gtk_widget_get_first_child(box), image) == UMI_STATUS_PERMISSION_DENIED);
    CHECK(UmiGtk4RecordingSetPrivate(box, 0) == UMI_STATUS_OK); Pump();
    CHECK(UmiGtk4RecordingPanelShowAt(window, "umicom.notes", "relative") == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiGtk4RecordingPanelShowAt(window, "umicom.notes", directory) == UMI_STATUS_OK); Pump();
    panel = Recorder(); CHECK(panel != NULL);
    start = Find(GTK_WIDGET(panel), "recording.start"); note = Find(GTK_WIDGET(panel), "recording.note");
    add = Find(GTK_WIDGET(panel), "recording.add-note"); export = Find(GTK_WIDGET(panel), "recording.export");
    CHECK(start != NULL && note != NULL && add != NULL && export != NULL);
    /* This is widget-signal integration, not an OS mouse-input test. */
    g_signal_emit_by_name(start, "clicked"); gtk_editable_set_text(GTK_EDITABLE(note), "Save did not update the Notes file.");
    g_signal_emit_by_name(add, "clicked"); g_signal_emit_by_name(export, "clicked");
    existing = g_strdup(gtk_label_get_text(GTK_LABEL(Find(GTK_WIDGET(panel), "recording.status"))));
    CHECK(strstr(existing, "Reports saved") != NULL); g_free(existing);
    g_object_ref(start); gtk_window_destroy(window); Pump();
    /* A retained control must not access a released recording context. */
    g_signal_emit_by_name(start, "clicked"); g_object_unref(start); g_object_unref(panel);
    (void)g_remove(image); g_free(image);
    printf("Native fixture reports retained for inspection: %s\n", directory); g_free(directory);
    return 0;
}
