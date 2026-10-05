/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/creative_audition_gtk4.c
 * PURPOSE: Own a sanitized audio preview independently from mutable edit controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/creative_audition.h"
#include <stdlib.h>
#include <string.h>
typedef struct Audition
{
    GtkWidget *root;              /* Weak identity; signal closures follow its lifetime. */
    GtkWidget *controls, *status; /* Owned so retained children stay safe during disposal. */
    GtkMediaStream *stream;
    UmiCreativeAudioInfo info;
    bool busy, clear_requested, clearing, closing;
} Audition;
static Audition *AuditionState(GtkWidget *root)
{
    return GTK_IS_WIDGET(root) ? g_object_get_data(G_OBJECT(root), "umicom-creative-audition") : NULL;
}
static void AuditionTag(GtkWidget *widget, const char *id)
{
    g_object_set_data_full(G_OBJECT(widget), "umicom-automation-id", g_strdup(id), g_free);
}
/* Break both the controls' reference and the backend's callbacks before freeing
 * state. A caller retaining a child control cannot keep a retired preview live. */
static void AuditionRetire(Audition *state)
{
    GtkMediaStream *stream = state->stream;
    state->stream = NULL;
    if (stream != NULL)
    {
        g_signal_handlers_disconnect_by_data(stream, state->root);
        gtk_media_stream_pause(stream);
    }
    gtk_media_controls_set_media_stream(GTK_MEDIA_CONTROLS(state->controls), NULL);
    g_clear_object(&stream);
    memset(&state->info, 0, sizeof(state->info));
}
static void AuditionDispose(gpointer data)
{
    Audition *state = data;
    state->closing = true;
    state->busy = true;
    AuditionRetire(state);
    g_clear_object(&state->controls);
    g_clear_object(&state->status);
    g_free(state);
}
static void AuditionDescribe(Audition *state)
{
    const GError *error = state->stream != NULL ? gtk_media_stream_get_error(state->stream) : NULL;
    char *text;
    if (state->stream == NULL)
        text = g_strdup("No audio preview is loaded.");
    else if (error != NULL)
        text = g_strdup_printf("Playback backend unavailable: %s. "
                               "The edit and WAVE export remain available. Check the installed GTK media "
                               "backend and audio device.",
                               error->message);
    else if (!gtk_media_stream_is_prepared(state->stream))
        text = g_strdup("Preparing the local audio preview. Playback starts only when you press Play.");
    else
        text = g_strdup_printf("%u Hz, %u channel(s), %.3f seconds. %s "
                               "Playback volume does not change the exported samples.",
                               state->info.sampleRate, (unsigned)state->info.channels,
                               (double)state->info.frames / (double)state->info.sampleRate,
                               gtk_media_stream_get_playing(state->stream) ? "Playing."
                                                                           : "Paused; press Play to listen.");
    gtk_label_set_text(GTK_LABEL(state->status), text);
    g_free(text);
}
static void AuditionFinish(GtkWidget *root, Audition *state)
{
    state->busy = false;
    if (state->clear_requested)
    {
        state->clear_requested = false;
        UmiCreativeAuditionGtkClear(root);
    }
    g_object_unref(root);
}
static void AuditionChanged(GObject *object, GParamSpec *property, gpointer context)
{
    (void)property;
    GtkWidget *root = context;
    Audition *state = AuditionState(root);
    if (state == NULL || state->closing || state->busy || G_OBJECT(state->stream) != object)
        return;
    g_object_ref(root);
    state->busy = true;
    /* A retained stream may receive external property edits. It still must not
     * produce background sound after the owning panel has been hidden. */
    if (!gtk_widget_get_mapped(root))
        gtk_media_stream_pause(state->stream);
    AuditionDescribe(state);
    AuditionFinish(root, state);
}
static void AuditionMap(GtkWidget *root, gpointer context)
{
    (void)context;
    Audition *state = AuditionState(root);
    if (state == NULL || state->closing || state->busy)
        return;
    g_object_ref(root);
    state->busy = true;
    if (state->stream != NULL)
    {
        gtk_media_stream_pause(state->stream);
        if (gtk_widget_get_mapped(root))
            gtk_media_controls_set_media_stream(GTK_MEDIA_CONTROLS(state->controls), state->stream);
    }
    AuditionDescribe(state);
    AuditionFinish(root, state);
}
static void AuditionUnmap(GtkWidget *root, gpointer context)
{
    (void)context;
    Audition *state = AuditionState(root);
    if (state == NULL || state->closing)
        return;
    g_object_ref(root);
    bool already_busy = state->busy;
    state->busy = true;
    if (state->stream != NULL)
        gtk_media_stream_pause(state->stream);
    gtk_media_controls_set_media_stream(GTK_MEDIA_CONTROLS(state->controls), NULL);
    if (already_busy)
        g_object_unref(root);
    else
        AuditionFinish(root, state);
}
GtkWidget *UmiCreativeAuditionGtkCreate(void)
{
    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    Audition *state = g_new0(Audition, 1);
    state->root = root;
    state->controls = gtk_media_controls_new(NULL);
    state->status = gtk_label_new("No audio preview is loaded. Loading never starts playback.");
    g_object_ref_sink(state->controls);
    g_object_ref_sink(state->status);
    gtk_label_set_wrap(GTK_LABEL(state->status), TRUE);
    gtk_label_set_xalign(GTK_LABEL(state->status), 0.0F);
    AuditionTag(root, "creative.audition");
    AuditionTag(state->controls, "creative.audition.controls");
    AuditionTag(state->status, "creative.audition.status");
    gtk_box_append(GTK_BOX(root), state->controls);
    gtk_box_append(GTK_BOX(root), state->status);
    g_object_set_data_full(G_OBJECT(root), "umicom-creative-audition", state, AuditionDispose);
    g_signal_connect(root, "map", G_CALLBACK(AuditionMap), NULL);
    g_signal_connect(root, "unmap", G_CALLBACK(AuditionUnmap), NULL);
    return root;
}
UmiStatus UmiCreativeAuditionGtkLoadWave(GtkWidget *root, const void *bytes, size_t length)
{
    Audition *state = AuditionState(root);
    if (state == NULL || state->closing)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (state->busy)
        return UMI_STATUS_BUSY;
    g_object_ref(root);
    state->busy = true;
    UmiCreativeAudioClip *clip = NULL;
    UmiCreativeAudioInfo info = {0};
    UmiCreativeExport canonical = {0};
    UmiStatus status = UmiCreativeAudioDecode(bytes, length, &clip);
    if (status == UMI_STATUS_OK)
        status = UmiCreativeAudioGetInfo(clip, &info);
    UmiCreativeAudioEdit whole = {0U, info.frames, 1000U, 0U, 0U};
    if (status == UMI_STATUS_OK)
        status = UmiCreativeAudioRender(clip, &whole, &canonical);
    UmiCreativeAudioDestroy(clip);
    if (status == UMI_STATUS_OK)
    {
        /* The shared renderer emits only PCM fmt/data chunks. Backend input has
         * no filename, remote location or optional metadata to interpret. */
        GBytes *owned = g_bytes_new(canonical.bytes, canonical.size);
        GInputStream *input = g_memory_input_stream_new_from_bytes(owned);
        GtkMediaStream *stream = gtk_media_file_new_for_input_stream(input);
        g_object_unref(input);
        g_bytes_unref(owned);
        if (stream == NULL)
            status = UMI_STATUS_UNAVAILABLE;
        else
        {
            gtk_media_stream_set_volume(stream, 0.20);
            gtk_media_stream_set_loop(stream, FALSE);
            gtk_media_stream_pause(stream);
            AuditionRetire(state);
            state->stream = stream;
            state->info = info;
            const char *signals[] = {"notify::prepared", "notify::error", "notify::playing", "notify::ended"};
            for (size_t i = 0U; i < sizeof(signals) / sizeof(signals[0]); ++i)
                g_signal_connect_object(stream, signals[i], G_CALLBACK(AuditionChanged), root, 0);
            if (gtk_widget_get_mapped(root))
                gtk_media_controls_set_media_stream(GTK_MEDIA_CONTROLS(state->controls), stream);
            AuditionDescribe(state);
        }
    }
    UmiCreativeExportFree(&canonical);
    AuditionFinish(root, state);
    return status;
}
void UmiCreativeAuditionGtkClear(GtkWidget *root)
{
    Audition *state = AuditionState(root);
    if (state == NULL || state->closing)
        return;
    if (state->busy)
    {
        if (!state->clearing)
            state->clear_requested = true;
        return;
    }
    g_object_ref(root);
    state->busy = true;
    state->clearing = true;
    AuditionRetire(state);
    AuditionDescribe(state);
    state->clearing = false;
    AuditionFinish(root, state);
}
UmiStatus UmiCreativeAuditionGtkRead(GtkWidget *root, UmiCreativeAuditionState *out)
{
    Audition *state = AuditionState(root);
    if (state == NULL || state->closing || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiCreativeAuditionState value = {0};
    if (state->stream != NULL)
    {
        value.loaded = true;
        value.prepared = gtk_media_stream_is_prepared(state->stream) != FALSE;
        value.playing = gtk_media_stream_get_playing(state->stream) != FALSE;
        value.seekable = gtk_media_stream_is_seekable(state->stream) != FALSE;
        value.backend_error = gtk_media_stream_get_error(state->stream) != NULL;
        value.volume = gtk_media_stream_get_volume(state->stream);
        value.source_frames = state->info.frames;
        value.sample_rate = state->info.sampleRate;
        value.channels = state->info.channels;
    }
    *out = value;
    return UMI_STATUS_OK;
}
