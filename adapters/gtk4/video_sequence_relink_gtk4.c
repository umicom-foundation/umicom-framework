/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/video_sequence_relink_gtk4.c
 * PURPOSE: Expose reviewed source relinking through the shared sequence and undo owner.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "video_sequence_private.h"
#include "umicom/creative_workspace/video_relink.h"
#include <string.h>

void UmiVideoSequenceGtkRelink(GtkButton *button, gpointer root) {
  (void)button;
  UmiVideoSequencePanel *panel = UmiVideoSequenceGtkState(GTK_WIDGET(root));
  if (panel->closed || panel->busy || panel->refreshing || panel->changing)
    return;
  panel->changing = true;
  const UmiVideoSequence *sequence = UmiVideoSequenceEditorRead(panel->editor);
  guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->selector));
  if ((size_t)selected >= sequence->clip_count) {
    UmiVideoSequenceGtkMessage(panel, "Select an added clip before relinking its source.");
    panel->changing = false;
    return;
  }
  /* Only the selected recipe value supplies IDs and trim ranges. Unapplied
   * text in the general editing form must not become part of a relink. */
  bool all = gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->relinkAll));
  const char *expected = sequence->clips[selected].path;
  char *replacement = g_strdup(gtk_editable_get_text(GTK_EDITABLE(panel->relinkPath)));
  UmiVideoSequenceRelink requests[UMI_VIDEO_SEQUENCE_MAX_CLIPS];
  size_t count = 0U;
  for (size_t i = 0U; i < sequence->clip_count; ++i) {
    if (i == selected || (all && strcmp(sequence->clips[i].path, expected) == 0)) {
      requests[count++] = (UmiVideoSequenceRelink){
          sequence->clips[i].id, sequence->clips[i].path, replacement};
    }
  }
  UmiStatus status = UmiVideoSequenceEditorRelink(panel->editor, requests, count, NULL);
  g_free(replacement);
  if (status == UMI_STATUS_OK) {
    UmiVideoSequenceGtkStopPreview(panel);
    panel->dirty = !UmiVideoSequenceEqual(UmiVideoSequenceEditorRead(panel->editor), &panel->saved);
    if (!panel->closed) {
      gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->approval), FALSE);
      UmiVideoSequenceGtkRefresh(panel);
      UmiVideoSequenceGtkMessage(panel,
          "Source references updated. Timing and order are unchanged. Preview the new media, "
          "then save a new recipe. Undo restores the previous references.");
    }
  } else {
    char *message = g_strdup_printf("Relink not applied: %s. Choose an absolute local path; "
                                   "the recipe and its history are unchanged.", umi_status_text(status));
    UmiVideoSequenceGtkMessage(panel, message);
    g_free(message);
  }
  panel->changing = false;
}
