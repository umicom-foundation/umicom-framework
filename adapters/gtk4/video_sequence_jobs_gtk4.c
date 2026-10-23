/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/video_sequence_jobs_gtk4.c
 * PURPOSE: Capture media jobs before dispatch and publish results only to a live editor.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/creative_workspace/video_captions.h"
#include "umicom/creative_workspace/video_soundtrack.h"
#include "umicom/platform/input_file.h"
#include "video_sequence_private.h"
#include <string.h>
typedef enum JobKind {
  OPEN_RECIPE,
  SAVE_RECIPE,
  RENDER_VIDEO,
  ATTACH_SOUNDTRACK,
  ATTACH_CAPTIONS
} JobKind;
/* All media-producing jobs share one cancellation and output owner. Adding a
 * preparation policy must not introduce a second process or file lifecycle. */
static bool RenderJob(JobKind kind) {
  return kind == RENDER_VIDEO || kind == ATTACH_SOUNDTRACK || kind == ATTACH_CAPTIONS;
}
typedef struct VideoJob {
  GWeakRef root;
  JobKind kind;
  UmiVideoSequence sequence;
  UmiVideoRenderPlan *render;
  UmiCancellationToken *cancel;
  char *path;
  UmiStatus status;
  UmiVideoRenderReport rendered;
  UmiCreativeAssetWriteResult saved;
} VideoJob;
static void FreeJob(gpointer data) {
  VideoJob *job = data;
  g_weak_ref_clear(&job->root);
  UmiVideoRenderPlanDestroy(job->render);
  umi_cancellation_token_destroy(job->cancel);
  g_free(job->path);
  g_free(job);
}
static void Work(GTask *task, gpointer source, gpointer taskData, GCancellable *cancel) {
  (void)source;
  (void)cancel;
  VideoJob *job = taskData;
  UmiCreativeAsset *asset = NULL;
  if (umi_cancellation_token_is_requested(job->cancel))
    job->status = UMI_STATUS_CANCELLED;
  else if (RenderJob(job->kind))
    job->status = UmiVideoRenderExecute(job->render, job->path, UMI_VIDEO_RENDER_MAX_TIMEOUT_MS,
                                        job->cancel, &job->rendered);
  else if (job->kind == SAVE_RECIPE) {
    job->status = UmiVideoSequenceExport(&job->sequence, job->cancel, &asset);
    if (job->status == UMI_STATUS_OK)
      job->status = UmiCreativeAssetWriteNew(asset, job->path, job->cancel, &job->saved);
  } else {
    unsigned char *bytes = NULL;
    size_t size = 0U;
    job->status = UmiInputFileRead(job->path, UMI_VIDEO_SEQUENCE_DOCUMENT_BYTES, &bytes, &size);
    if (job->status == UMI_STATUS_OK)
      job->status = UmiVideoSequenceImport(bytes, size, job->cancel, &job->sequence);
    UmiInputFileFree(bytes);
  }
  UmiCreativeAssetDestroy(asset);
  g_task_return_boolean(task, TRUE);
}
static void Complete(GObject *source, GAsyncResult *result, gpointer data) {
  (void)source;
  (void)data;
  VideoJob *job = g_task_get_task_data(G_TASK(result));
  GtkWidget *root = g_weak_ref_get(&job->root);
  if (root == NULL)
    return;
  UmiVideoSequencePanel *panel = UmiVideoSequenceGtkState(root);
  /* Parsing is provisional until the GTK owner accepts it. A cancellation
   * arriving after worker success must still preserve the current recipe.
   * Save/render results describe already-created files and are not rewritten. */
  if (job->kind == OPEN_RECIPE && job->status == UMI_STATUS_OK &&
      umi_cancellation_token_is_requested(job->cancel))
    job->status = UMI_STATUS_CANCELLED;
  /* Clear the borrowed token before GTask releases it, including for a
   * retired but retained widget. Late results never resurrect its controls. */
  if (panel->cancel == job->cancel)
    panel->cancel = NULL;
  /* Keep dispatch closed through every notifying setter in completion. */
  if (!panel->closed) {
    if (job->status == UMI_STATUS_OK && job->kind == OPEN_RECIPE) {
      /* Opening replaces history only after a complete parse and a
       * successful replacement allocation. Failed opens preserve edits. */
      UmiVideoSequenceEditor *next = NULL;
      job->status = UmiVideoSequenceEditorCreate(&job->sequence, &next);
      if (job->status == UMI_STATUS_OK) {
        UmiVideoSequenceEditorDestroy(panel->editor);
        panel->editor = next;
        panel->saved = job->sequence;
        panel->dirty = false;
      }
    }
    if (job->status == UMI_STATUS_OK && job->kind == SAVE_RECIPE) {
      panel->saved = job->sequence;
      panel->dirty = false;
    }
    gtk_widget_set_sensitive(panel->form, TRUE);
    if (panel->closed) goto finished;
    gtk_widget_set_sensitive(panel->cancelButton, FALSE);
    if (panel->closed) goto finished;
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->approval), FALSE);
    if (panel->closed) goto finished;
    UmiVideoSequenceGtkRefresh(panel);
    if (panel->closed) goto finished;
    const char *operation = job->kind == OPEN_RECIPE         ? "Open recipe"
                            : job->kind == SAVE_RECIPE       ? "Save recipe"
                            : job->kind == ATTACH_CAPTIONS   ? "Attach captions"
                            : job->kind == ATTACH_SOUNDTRACK ? "Attach soundtrack"
                                                             : "Render silent video";
    bool created = RenderJob(job->kind) ? job->rendered.output_created : job->saved.created;
    char *message =
        g_strdup_printf("%s: %s.\n%s\n%s%s", operation, umi_status_text(job->status), job->path,
                        job->status != UMI_STATUS_OK && created
                            ? "A new partial file remains. Choose another output path.\n"
                            : "",
                        RenderJob(job->kind) && job->status == UMI_STATUS_OK
                            ? "Render finished. Open the MP4 in a player to verify its content."
                            : "");
    UmiVideoSequenceGtkMessage(panel, message);
    g_free(message);
    if (!panel->closed && RenderJob(job->kind) && job->rendered.process.diagnostics[0] != '\0') {
      /* Diagnostics are text, never GTK markup. Keep the process owner's
       * bounded tail visible so a missing codec has an actionable cause. */
      /* Retain the borrowed-buffer update for ownership review. The replacement
       * below holds the buffer and refreshes its iterator across notifications. */
#if 0
      GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->summary));
      GtkTextIter end;
      gtk_text_buffer_get_end_iter(buffer, &end);
      gtk_text_buffer_insert(buffer, &end, "\nRenderer diagnostics:\n", -1);
      char *valid = g_utf8_make_valid(job->rendered.process.diagnostics, -1);
      gtk_text_buffer_insert(buffer, &end, valid, -1);
      g_free(valid);
#endif
      /* A host can retire the root from a buffer observer. Hold the buffer
       * across notifications and re-acquire iterators after each insertion. */
      GtkTextBuffer *buffer = g_object_ref(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->summary)));
      GtkTextIter end;
      gtk_text_buffer_get_end_iter(buffer, &end);
      gtk_text_buffer_insert(buffer, &end, "\nRenderer diagnostics:\n", -1);
      char *valid = g_utf8_make_valid(job->rendered.process.diagnostics, -1);
      if (!panel->closed) {
        gtk_text_buffer_get_end_iter(buffer, &end);
        gtk_text_buffer_insert(buffer, &end, valid, -1);
      }
      g_free(valid);
      g_object_unref(buffer);
    }
  }
finished:
  panel->busy = false;
  g_object_unref(root);
}
void UmiVideoSequenceGtkJob(GtkButton *button, gpointer data) {
  GtkWidget *root = GTK_WIDGET(data);
  UmiVideoSequencePanel *panel = UmiVideoSequenceGtkState(root);
  if (panel->closed || panel->busy || panel->refreshing || panel->changing)
    return;
  panel->changing = true;
  const char *action = g_object_get_data(G_OBJECT(button), "video-action");
  JobKind kind = strcmp(action, "captions") == 0     ? ATTACH_CAPTIONS
                 : strcmp(action, "soundtrack") == 0 ? ATTACH_SOUNDTRACK
                 : strcmp(action, "render") == 0     ? RENDER_VIDEO
                 : strcmp(action, "open") == 0       ? OPEN_RECIPE
                                                     : SAVE_RECIPE;
  if (kind == OPEN_RECIPE && panel->dirty &&
      !gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->approval))) {
    UmiVideoSequenceGtkMessage(
        panel, "Save your edits, or confirm replacement before opening another recipe.");
    panel->changing = false;
    return;
  }
  VideoJob *job = g_new0(VideoJob, 1);
  g_weak_ref_init(&job->root, root);
  job->kind = kind;
  job->sequence = *UmiVideoSequenceEditorRead(panel->editor);
  job->path = g_strdup(
      gtk_editable_get_text(GTK_EDITABLE(kind == ATTACH_CAPTIONS     ? panel->captionOutput
                                         : kind == ATTACH_SOUNDTRACK ? panel->soundtrackOutput
                                         : kind == RENDER_VIDEO      ? panel->output
                                                                     : panel->projectPath)));
  UmiStatus status = UmiOutputFileValidatePath(job->path);
  if (status == UMI_STATUS_OK && RenderJob(kind)) {
    char directory[UMI_PATH_CAPACITY];
    status = umi_path_parent(job->path, directory, sizeof(directory));
    if (status == UMI_STATUS_OK) {
      const char *program = gtk_editable_get_text(GTK_EDITABLE(panel->program));
      if (kind == ATTACH_CAPTIONS)
        status = UmiVideoCaptionsPrepare(gtk_editable_get_text(GTK_EDITABLE(panel->captionVideo)),
                                         gtk_editable_get_text(GTK_EDITABLE(panel->captionSource)),
                                         program, directory, &job->render);
      else if (kind == ATTACH_SOUNDTRACK)
        status =
            UmiVideoSoundtrackPrepare(gtk_editable_get_text(GTK_EDITABLE(panel->soundtrackVideo)),
                                      gtk_editable_get_text(GTK_EDITABLE(panel->soundtrackAudio)),
                                      program, directory, &job->render);
      else
        status = UmiVideoRenderPrepare(&job->sequence, program, directory, &job->render);
    }
  }
  if (status == UMI_STATUS_OK)
    status = umi_cancellation_token_create(&job->cancel);
  if (status != UMI_STATUS_OK) {
    char *message =
        g_strdup_printf("Operation not started: %s. Choose absolute paths and valid clips.",
                        umi_status_text(status));
    UmiVideoSequenceGtkMessage(panel, message);
    g_free(message);
    FreeJob(job);
    panel->changing = false;
    return;
  }
  /* Capture every input and close the dispatch gate before any GTK setter
   * can notify observers. Retained controls cannot submit a second job. */
  panel->busy = true;
  panel->changing = false;
  panel->cancel = job->cancel;
  UmiVideoSequenceGtkStopPreview(panel);
  if (panel->closed) goto cancelled_before_dispatch;
  gtk_widget_set_sensitive(panel->form, FALSE);
  if (panel->closed) goto cancelled_before_dispatch;
  gtk_widget_set_sensitive(panel->cancelButton, TRUE);
  if (panel->closed) goto cancelled_before_dispatch;
  UmiVideoSequenceGtkMessage(panel, "Working with the captured recipe and selected path...");
  if (panel->closed) goto cancelled_before_dispatch;
  GTask *task = g_task_new(NULL, NULL, Complete, NULL);
  g_task_set_task_data(task, job, FreeJob);
  g_task_run_in_thread(task, Work);
  g_object_unref(task);
  return;
cancelled_before_dispatch:
  /* A native observer closed the owner before any worker accepted the job. */
  panel->cancel = NULL;
  panel->busy = false;
  FreeJob(job);
}
