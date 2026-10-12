/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/video_sequence_preview_gtk4.c
 * PURPOSE: Preview a selected saved trim through GTK without modifying source media.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "video_sequence_private.h"
#include <string.h>
#ifdef UMI_CREATIVE_HAS_GSTREAMER
#include <gst/gst.h>
#endif
void UmiVideoSequenceGtkStopPreview(UmiVideoSequencePanel *panel) {
  if (panel->stream == NULL)
    return;
  GtkMediaStream *stream = panel->stream;
  panel->stream = NULL;
  panel->waitingForSeek = false;
  panel->prepared = false;
  /* Disconnect first: pausing and detaching can synchronously publish media
   * properties. No old stream may update a newly selected clip's state. */
  g_signal_handlers_disconnect_by_data(stream, panel->root);
  gtk_media_stream_pause(stream);
  gtk_video_set_media_stream(GTK_VIDEO(panel->video), NULL);
  g_object_unref(stream);
}
static void Changed(GtkMediaStream *stream, GParamSpec *property, gpointer root);
static void InspectStream(GtkMediaStream *stream, GParamSpec *property, gpointer root) {
  (void)property;
  UmiVideoSequencePanel *panel = UmiVideoSequenceGtkState(GTK_WIDGET(root));
  if (panel->closed || panel->stream != stream)
    return;
  const GError *error = gtk_media_stream_get_error(stream);
  if (error != NULL) {
    UmiVideoSequenceGtkMessage(panel, error->message);
    UmiVideoSequenceGtkStopPreview(panel);
    return;
  }
  if (!gtk_media_stream_is_prepared(stream))
    return;
  gint64 begin = (gint64)panel->previewBegin * 1000;
  gint64 end = (gint64)panel->previewEnd * 1000;
  if (!panel->prepared) {
    if (!gtk_media_stream_is_seekable(stream) || gtk_media_stream_get_duration(stream) < end) {
      UmiVideoSequenceGtkMessage(panel,
                                 "Preview requires a seekable video with the full selected range.");
      UmiVideoSequenceGtkStopPreview(panel);
      return;
    }
    panel->prepared = true;
    panel->waitingForSeek = true;
    gtk_media_stream_seek(stream, begin);
    /* A backend may complete a seek synchronously, including a seek to zero. */
    if (panel->stream == stream && !gtk_media_stream_is_seeking(stream))
      Changed(stream, NULL, root);
    return;
  }
  if (panel->waitingForSeek && !gtk_media_stream_is_seeking(stream)) {
    panel->waitingForSeek = false;
    if (gtk_media_stream_get_timestamp(stream) < begin ||
        gtk_media_stream_get_timestamp(stream) >= end) {
      UmiVideoSequenceGtkMessage(panel,
                                 "The media backend could not seek into the selected range.");
      UmiVideoSequenceGtkStopPreview(panel);
      return;
    }
    gtk_media_stream_play(stream);
    return;
  }
  /* Native video controls can seek as well. Pause outside the selected range;
   * checking playing first prevents pause notifications from recursing forever. */
  if (!panel->waitingForSeek && !gtk_media_stream_is_seeking(stream) &&
      gtk_media_stream_get_playing(stream) &&
      (gtk_media_stream_get_timestamp(stream) < begin ||
       gtk_media_stream_get_timestamp(stream) >= end)) {
    gtk_media_stream_pause(stream);
    UmiVideoSequenceGtkMessage(panel, "Selected clip preview finished.");
  }
}
/* GTK property changes may synchronously invoke application observers.
 * Retain the notifying stream and keep user actions gated until inspection ends. */
static void Changed(GtkMediaStream *stream, GParamSpec *property, gpointer root) {
  UmiVideoSequencePanel *panel = UmiVideoSequenceGtkState(GTK_WIDGET(root));
  bool changing = panel->changing;
  panel->changing = true;
  g_object_ref(stream);
  InspectStream(stream, property, root);
  g_object_unref(stream);
  panel->changing = changing;
}
void UmiVideoSequenceGtkPreview(GtkButton *button, gpointer root) {
  UmiVideoSequencePanel *panel = UmiVideoSequenceGtkState(GTK_WIDGET(root));
  if (panel->closed || panel->busy || panel->refreshing || panel->changing)
    return;
  panel->changing = true;
  UmiVideoSequenceGtkStopPreview(panel);
  /* Detaching the previous stream can notify an observer that closes the tool.
   * Respect retirement before creating another media resource. */
  if (panel->closed)
    goto complete;
  if (strcmp(g_object_get_data(G_OBJECT(button), "video-action"), "stop") == 0)
    goto complete;
  const UmiVideoSequence *sequence = UmiVideoSequenceEditorRead(panel->editor);
  guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->selector));
  if ((size_t)selected >= sequence->clip_count) {
    UmiVideoSequenceGtkMessage(panel, "Select an added clip first.");
    goto complete;
  }
  UmiVideoSequenceClip value = sequence->clips[selected];
  const UmiVideoSequenceClip *clip = &value;
  /* Check the backend before constructing GtkMediaFile: a missing playbin3
   * can abort inside GTK before a media error can be reported to the user. */
#ifdef UMI_CREATIVE_HAS_GSTREAMER
  const char *backend = g_getenv("GTK_MEDIA");
  if ((backend != NULL && backend[0] != '\0' && strcmp(backend, "gstreamer") != 0) ||
      !gst_init_check(NULL, NULL, NULL)) {
    UmiVideoSequenceGtkMessage(panel, "Video preview requires the GStreamer GTK backend.");
    goto complete;
  }
  const char *required[] = {"playbin3",     "giostreamsrc", "appsink",
                            "videoconvert", "videoscale",   "autoaudiosink"};
  for (size_t i = 0U; i < sizeof(required) / sizeof(required[0]); ++i) {
    GstElementFactory *factory = gst_element_factory_find(required[i]);
    if (factory == NULL) {
      UmiVideoSequenceGtkMessage(
          panel, "Install the matching GStreamer playback plugins to preview video.");
      goto complete;
    }
    gst_object_unref(factory);
  }
#else
  UmiVideoSequenceGtkMessage(panel, "Video preview was built without GStreamer capability checks. "
                                    "Editing and rendering remain available.");
  goto complete;
#endif
  panel->previewBegin = clip->begin_ms;
  panel->previewEnd = clip->end_ms;
  /* Construct empty so property observers are installed before file loading.
   * Muting matches the silent render contract; audio editing has its own tool. */
  GtkMediaStream *stream = gtk_media_file_new();
  panel->stream = g_object_ref(stream);
  gtk_media_stream_set_muted(panel->stream, TRUE);
  g_signal_connect_object(panel->stream, "notify", G_CALLBACK(Changed), panel->root, 0);
  gtk_video_set_media_stream(GTK_VIDEO(panel->video), panel->stream);
  GFile *file = g_file_new_for_path(clip->path);
  if (!panel->closed && panel->stream == stream)
    gtk_media_file_set_file(GTK_MEDIA_FILE(stream), file);
  g_object_unref(file);
  g_object_unref(stream);
complete:
  panel->changing = false;
}
