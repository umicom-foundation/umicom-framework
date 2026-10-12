/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/developer_launch_inspection.inc
 * PURPOSE: Read launch-file facts on a worker while rejecting stale or retired forms.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/launch_inspection.h"

typedef struct DeveloperLaunchInspection {
  UmiBuildProfile profile;
  UmiBuildLaunchInspection observation;
  UmiCancellationToken *cancel;
  UmiStatus status;
} DeveloperLaunchInspection;

static void DeveloperLaunchInspectionFree(gpointer data) {
  DeveloperLaunchInspection *request = data;
  umi_cancellation_token_destroy(request->cancel);
  g_free(request);
}
static void DeveloperLaunchInspectionWorker(GTask *task, gpointer source, gpointer data,
                                            GCancellable *cancel) {
  (void)source;
  (void)cancel;
  DeveloperLaunchInspection *request = data;
  request->status = UmiBuildLaunchInspect(&request->profile, request->profile.source_directory,
                                          request->cancel, &request->observation);
  g_task_return_boolean(task, TRUE);
}
/* File types are observations, not promises that the loader will accept a
 * program. Keep both paths visible because bare Run names and Debug differ. */
static void DeveloperLaunchInspectionRow(GString *text, const char *label,
                                         const UmiBuildLaunchFile *file) {
  g_string_append_printf(text, "%s: %s\n", label, file->path);
  if (!file->inspected)
    g_string_append(text, "  Not inspected: this Run name uses operating-system program lookup.\n");
  else if (file->status != UMI_STATUS_OK)
    g_string_append_printf(text, "  File check: %s.\n", umi_status_text(file->status));
  else {
    const char *kind = file->kind == UMI_FILE_KIND_REGULAR     ? "regular file"
                       : file->kind == UMI_FILE_KIND_DIRECTORY ? "directory"
                       : file->kind == UMI_FILE_KIND_SYMBOLIC_LINK
                           ? "symbolic link or reparse point"
                           : "other file type";
    g_string_append_printf(
        text, "  %s; %" G_GUINT64_FORMAT " bytes. %s\n", kind, (guint64)file->size,
        file->expected_kind ? "Expected file type." : "Not the expected file type.");
  }
}
static void DeveloperLaunchInspectionDone(GObject *source, GAsyncResult *result, gpointer context) {
  (void)source;
  UmiGtk4DeveloperDialog *dialog = context;
  DeveloperLaunchInspection *request = g_task_get_task_data(G_TASK(result));
  (void)g_task_propagate_boolean(G_TASK(result), NULL);
  dialog->launchInspectCancel = NULL;
  /* The busy flag remains set during publication, including notification
   * callbacks that try to launch another check or apply settings. */
  if (DeveloperVisible(dialog)) {
    GString *text = g_string_new(NULL);
    if (dialog->launchInvalidated)
      g_string_append(
          text,
          "Launch inputs changed during the file check. Check again using the current fields.");
    else if (request->status != UMI_STATUS_OK)
      g_string_append_printf(text, "Launch file check: %s. Existing settings are unchanged.",
                             umi_status_text(request->status));
    else {
      DeveloperLaunchInspectionRow(text, "Run program", &request->observation.run_program);
      DeveloperLaunchInspectionRow(text, "Native Debug program",
                                   &request->observation.debug_program);
      DeveloperLaunchInspectionRow(text, "Working folder", &request->observation.working_directory);
      g_string_append(text,
                      "These are current file observations only. Permissions, executable format, "
                      "dependencies and debugger compatibility are not verified. Nothing was "
                      "launched or saved.");
    }
    gtk_label_set_text(GTK_LABEL(dialog->launchDetail), text->str);
    g_string_free(text, TRUE);
    if (DeveloperVisible(dialog) && dialog->launchInvalidated)
      gtk_label_set_text(
          GTK_LABEL(dialog->launchDetail),
          "Launch inputs changed during the file check. Check again using the current fields.");
  }
  dialog->launchChanging = 0;
  DeveloperRelease(dialog);
}
static void DeveloperLaunchInspectionStart(GtkButton *button, gpointer context) {
  (void)button;
  UmiGtk4DeveloperDialog *dialog = DeveloperAcquire(context);
  if (dialog == NULL)
    return;
  if (dialog->launchChanging || dialog->launchChoosing || dialog->presetChanging ||
      dialog->presetLoading || dialog->installedLoading || dialog->installedChanging ||
      dialog->targetLoading || dialog->targetChanging || dialog->accepting) {
    DeveloperRelease(dialog);
    return;
  }
  DeveloperLaunchInspection *request = g_new0(DeveloperLaunchInspection, 1);
  request->status = DeveloperLaunchInputs(dialog, &request->profile);
  if (request->status == UMI_STATUS_OK)
    request->status = umi_cancellation_token_create(&request->cancel);
  dialog->launchChanging = 1;
  dialog->launchInvalidated = 0;
  dialog->launchInspectCancel = request->cancel;
  if (request->status != UMI_STATUS_OK) {
    gtk_label_set_text(
        GTK_LABEL(dialog->launchDetail),
        "Could not capture launch fields. Keep paths and arguments within the supported limits.");
    goto abandoned;
  }
  /* Capture before notifying. A form edit, even followed by restoring its
   * original text, invalidates this exact observation rather than a new one. */
  gtk_label_set_text(GTK_LABEL(dialog->launchDetail), "Checking launch files...");
  if (!DeveloperVisible(dialog))
    goto abandoned;
  GTask *task = g_task_new(NULL, NULL, DeveloperLaunchInspectionDone, dialog);
  g_task_set_task_data(task, request, DeveloperLaunchInspectionFree);
  g_task_run_in_thread(task, DeveloperLaunchInspectionWorker);
  g_object_unref(task);
  return; /* Completion owns the acquired controller reference. */
abandoned:
  dialog->launchInspectCancel = NULL;
  dialog->launchChanging = 0;
  DeveloperLaunchInspectionFree(request);
  DeveloperRelease(dialog);
}
