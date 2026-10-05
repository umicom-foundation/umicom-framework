/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/creative_audio/test_audition_gtk4.c
 * PURPOSE: Check preview ownership and lifecycle without starting audio playback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/creative_audition.h"
#include "umicom/ui/gtk4/creative_audio.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(value)                                                                                         \
    do                                                                                                       \
    {                                                                                                        \
        if (!(value))                                                                                        \
        {                                                                                                    \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value);                                      \
            failed = 1;                                                                                      \
            goto cleanup;                                                                                    \
        }                                                                                                    \
    } while (0)
/* Silence keeps this fixture inert even if a backend prepares a device. The
 * test never invokes Play and does not claim audible-output qualification. */
static const unsigned char wave[] = {
    'R', 'I', 'F', 'F', 40, 0,   0, 0, 'W', 'A', 'V', 'E', 'f', 'm', 't', ' ', 16, 0, 0, 0, 1, 0, 1, 0,
    128, 187, 0,   0,   0,  119, 1, 0, 2,   0,   16,  0,   'd', 'a', 't', 'a', 4,  0, 0, 0, 0, 0, 0, 0};
static GtkWidget *Find(GtkWidget *root, const char *id)
{
    const char *tag = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (tag != NULL && strcmp(tag, id) == 0)
        return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL;
         child = gtk_widget_get_next_sibling(child))
    {
        GtkWidget *found = Find(child, id);
        if (found != NULL)
            return found;
    }
    return NULL;
}
typedef struct Reentry
{
    GtkWidget *root;
    UmiStatus status;
    unsigned calls;
    bool clear;
} Reentry;
static void DuringPublication(GObject *object, GParamSpec *property, gpointer context)
{
    (void)property;
    Reentry *probe = context;
    if (gtk_media_controls_get_media_stream(GTK_MEDIA_CONTROLS(object)) == NULL)
        return;
    ++probe->calls;
    if (probe->clear)
        UmiCreativeAuditionGtkClear(probe->root);
    else
        probe->status = UmiCreativeAuditionGtkLoadWave(probe->root, wave, sizeof(wave));
}
static bool WaitMapped(GtkWidget *widget)
{
    gint64 deadline = g_get_monotonic_time() + 5 * G_TIME_SPAN_SECOND;
    while (!gtk_widget_get_mapped(widget) && g_get_monotonic_time() < deadline)
    {
        g_main_context_iteration(NULL, FALSE);
        g_usleep(1000U);
    }
    return gtk_widget_get_mapped(widget) != FALSE;
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    if (!gtk_init_check())
        return 77;
    const char *name = argv[1];
    int failed = 0;
    GtkWidget *root = NULL, *second = NULL, *panel = NULL, *controls = NULL;
    GtkWindow *window = GTK_WINDOW(gtk_window_new());
    g_object_ref(window);
    UmiCreativeAuditionState state;
    Reentry probe = {0};
    if (strcmp(name, "clip-invalidate") == 0 || strcmp(name, "clip-repreview") == 0 ||
        strcmp(name, "clip-bad-load") == 0)
    {
        panel = UmiCreativeAudioGtkCreate();
        g_object_ref_sink(panel);
        gtk_window_set_child(window, panel);
        gtk_window_present(window);
        CHECK(WaitMapped(panel));
        CHECK(UmiCreativeAudioGtkLoadBytes(panel, wave, sizeof(wave)) == UMI_STATUS_OK);
        g_signal_emit_by_name(Find(panel, "creative.audio.preview"), "clicked");
        g_signal_emit_by_name(Find(panel, "creative.audio.listen"), "clicked");
        GtkWidget *player = Find(panel, "creative.audition");
        CHECK(player != NULL);
        CHECK(UmiCreativeAuditionGtkRead(player, &state) == UMI_STATUS_OK && state.loaded && !state.playing);
        if (strcmp(name, "clip-invalidate") == 0)
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(Find(panel, "creative.audio.gain")), 500.0);
        else if (strcmp(name, "clip-repreview") == 0)
            g_signal_emit_by_name(Find(panel, "creative.audio.preview"), "clicked");
        else
            CHECK(UmiCreativeAudioGtkLoadBytes(panel, "bad", 3U) != UMI_STATUS_OK);
        CHECK(UmiCreativeAuditionGtkRead(player, &state) == UMI_STATUS_OK);
        CHECK(state.loaded == (strcmp(name, "clip-bad-load") == 0));
        goto cleanup;
    }
    root = UmiCreativeAuditionGtkCreate();
    g_object_ref_sink(root);
    controls = Find(root, "creative.audition.controls");
    CHECK(controls != NULL);
    g_object_ref(controls);
    CHECK(UmiCreativeAuditionGtkRead(root, &state) == UMI_STATUS_OK && !state.loaded && !state.playing);
    CHECK(gtk_media_controls_get_media_stream(GTK_MEDIA_CONTROLS(controls)) == NULL);
    if (strcmp(name, "empty") == 0)
        goto cleanup;
    gtk_window_set_child(window, root);
    gtk_window_present(window);
    CHECK(WaitMapped(root));
    probe.root = root;
    if (strcmp(name, "reentrant-load") == 0 || strcmp(name, "reentrant-clear") == 0)
    {
        probe.clear = strcmp(name, "reentrant-clear") == 0;
        g_signal_connect(controls, "notify::media-stream", G_CALLBACK(DuringPublication), &probe);
    }
    unsigned char supplied[sizeof(wave)];
    memcpy(supplied, wave, sizeof(wave));
    CHECK(UmiCreativeAuditionGtkLoadWave(root, supplied, sizeof(supplied)) == UMI_STATUS_OK);
    memset(supplied, 0xa5, sizeof(supplied)); /* Caller ownership ends immediately. */
    CHECK(UmiCreativeAuditionGtkRead(root, &state) == UMI_STATUS_OK);
    if (strcmp(name, "reentrant-clear") == 0)
    {
        CHECK(probe.calls == 1U && !state.loaded);
        goto cleanup;
    }
    if (strcmp(name, "reentrant-load") == 0)
        CHECK(probe.calls == 1U && probe.status == UMI_STATUS_BUSY);
    CHECK(state.loaded && !state.playing && state.source_frames == 2U && state.sample_rate == 48000U &&
          state.channels == 1U);
    CHECK(fabs(state.volume - 0.2) < 0.000001);
    GtkMediaStream *stream = gtk_media_controls_get_media_stream(GTK_MEDIA_CONTROLS(controls));
    CHECK(stream != NULL && GTK_IS_MEDIA_FILE(stream));
    CHECK(gtk_media_file_get_file(GTK_MEDIA_FILE(stream)) == NULL);
    CHECK(G_IS_MEMORY_INPUT_STREAM(gtk_media_file_get_input_stream(GTK_MEDIA_FILE(stream))));
    if (strcmp(name, "invalid-keeps-preview") == 0)
    {
        CHECK(UmiCreativeAuditionGtkLoadWave(root, "bad", 3U) != UMI_STATUS_OK);
        CHECK(gtk_media_controls_get_media_stream(GTK_MEDIA_CONTROLS(controls)) == stream);
    }
    if (strcmp(name, "independent") == 0)
    {
        second = UmiCreativeAuditionGtkCreate();
        g_object_ref_sink(second);
        CHECK(UmiCreativeAuditionGtkRead(second, &state) == UMI_STATUS_OK && !state.loaded);
    }
    if (strcmp(name, "clear") == 0)
    {
        UmiCreativeAuditionGtkClear(root);
        CHECK(UmiCreativeAuditionGtkRead(root, &state) == UMI_STATUS_OK && !state.loaded);
        CHECK(gtk_media_controls_get_media_stream(GTK_MEDIA_CONTROLS(controls)) == NULL);
    }
    if (strcmp(name, "hide-show") == 0)
    {
        gtk_widget_set_visible(root, FALSE);
        CHECK(gtk_media_controls_get_media_stream(GTK_MEDIA_CONTROLS(controls)) == NULL);
        CHECK(UmiCreativeAuditionGtkRead(root, &state) == UMI_STATUS_OK && !state.playing && state.loaded);
        gtk_widget_set_visible(root, TRUE);
        CHECK(WaitMapped(root));
        CHECK(gtk_media_controls_get_media_stream(GTK_MEDIA_CONTROLS(controls)) != NULL);
        CHECK(UmiCreativeAuditionGtkRead(root, &state) == UMI_STATUS_OK && !state.playing);
    }
    if (strcmp(name, "retained-controls") == 0)
    {
        gtk_window_set_child(window, NULL);
        g_clear_object(&root);
        CHECK(gtk_media_controls_get_media_stream(GTK_MEDIA_CONTROLS(controls)) == NULL);
    }
cleanup:
    if (controls != NULL)
        g_signal_handlers_disconnect_by_data(controls, &probe);
    gtk_window_set_child(window, NULL);
    g_clear_object(&root);
    g_clear_object(&panel);
    g_clear_object(&second);
    g_clear_object(&controls);
    gtk_window_destroy(window);
    g_object_unref(window);
    return failed;
}
