/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/education_inspection_gtk4.c
 * PURPOSE: Share explicit teaching-file inspection between Studio and education.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/education_inspection.h"
#include "umicom/ui/gtk4/text_export.h"
#include "umicom/ui/gtk4/path_picker.h"
#include "umicom/platform/input_file.h"
#include "umicom/education/schedule_trace.h"
#include "umicom/education/timer_trace.h"
#include "umicom/education/interrupt_trace.h"
#include "umicom/education/evidence_trace.h"
#include "umicom/education/device_io_trace.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef struct EducationInspection {
    GtkWidget *root, *path, *format, *status, *output;
    GtkWidget *inspect, *discard;
    GtkWidget *textExport; /* Owned control; no worker borrows the inspector. */
    bool closed, busy, discardResult;
} EducationInspection;
typedef struct EducationInspectionJob {
    GWeakRef root;
    char *path;
    guint format;
    UmiStatus status;
    char report[2048];
} EducationInspectionJob;
static void InspectionTag(GtkWidget *widget, const char *name)
{ g_object_set_data_full(G_OBJECT(widget), "umicom-automation-id", g_strdup(name), g_free); }
static void InspectionJobFree(gpointer data)
{
    EducationInspectionJob *job = data;
    g_weak_ref_clear(&job->root); g_free(job->path); g_free(job);
}
static void InspectBytes(EducationInspectionJob *job, const unsigned char *bytes, size_t length)
{
    /* Format is an explicit choice, not an inference from a filename suffix.
     * Each parser validates the entire teaching record sequence before a report
     * is formatted. No partially parsed result is sent to the main thread. */
    int written = -1;
    if (job->format == 0U) {
        UmiEducationScheduleReport report = {0};
        job->status = UmiEducationScheduleTraceInspect(bytes, length, &report);
        if (job->status == UMI_STATUS_OK) written = snprintf(job->report, sizeof(job->report),
            "Scheduler records: %" PRIu32 "\nAdmitted: %" PRIu32 "\nDispatched: %" PRIu32
            "\nReady: %" PRIu32 "\nWaiting: %" PRIu32 "\nRunning: %" PRIu32 "\nLast tick: %" PRIu64,
            report.recordCount, report.admitted, report.dispatched, report.readyAtEnd,
            report.waitingAtEnd, report.runningAtEnd, report.lastTick);
    } else if (job->format == 1U) {
        UmiEducationTimerReport report = {0};
        job->status = UmiEducationTimerTraceInspect(bytes, length, &report);
        if (job->status == UMI_STATUS_OK) written = snprintf(job->report, sizeof(job->report),
            "Timer records: %" PRIu32 "\nPending timers: %" PRIu32 "\nDistinct identities: %" PRIu32
            "\nLast tick: %" PRIu64 "\nNext finite deadline: %" PRIu64,
            report.recordCount, report.pendingAtEnd, report.distinctIdentities, report.lastTick, report.nextFiniteDeadline);
    } else if (job->format == 2U) {
        UmiEducationInterruptReport report = {0};
        job->status = UmiEducationInterruptTraceInspect(bytes, length, &report);
        if (job->status == UMI_STATUS_OK) written = snprintf(job->report, sizeof(job->report),
            "Interrupt records: %" PRIu32 "\nClaims: %" PRIu32 "\nEntries: %" PRIu32
            "\nAcknowledgements: %" PRIu32 "\nOwned: %" PRIu32 "\nActive: %" PRIu32 "\nFaulted: %" PRIu32,
            report.recordCount, report.claims, report.entries, report.acknowledgements,
            report.ownedAtEnd, report.activeAtEnd, report.faultedAtEnd);
    } else if (job->format == 3U) {
        UmiEducationEvidenceReport report = {0};
        job->status = UmiEducationEvidenceTraceInspect(bytes, length, &report);
        if (job->status == UMI_STATUS_OK) written = snprintf(job->report, sizeof(job->report),
            "Driver evidence records: %" PRIu32 "\nObservations: %" PRIu32 "\nCheckpoints: %" PRIu32
            "\nRecoveries recorded: %" PRIu32 "\nLive: %" PRIu32 "\nFaulted: %" PRIu32
            "\n\nDigest links are consistent. This is not signature authentication or proof of device recovery.",
            report.recordCount, report.observations, report.checkpoints, report.recoveries, report.liveAtEnd, report.faultedAtEnd);
    } else if (job->format == 4U) {
        /* Cancellation intent and acknowledged completion have different
         * meanings. Show both so a learner can inspect the race explicitly. */
        UmiEducationDeviceIoReport report = {0};
        job->status = UmiEducationDeviceIoTraceInspect(bytes, length, &report);
        if (job->status == UMI_STATUS_OK) written = snprintf(job->report, sizeof(job->report),
            "Device I/O records: %" PRIu32 "\nSubmitted: %" PRIu32 "\nCompleted: %" PRIu32
            "\nCancellation requests: %" PRIu32 "\nAcknowledged cancellations: %" PRIu32
            "\nOutstanding: %" PRIu32 "\nReady to reap: %" PRIu32
            "\nBytes requested: %" PRIu64 "\nBytes completed: %" PRIu64,
            report.recordCount, report.submissions, report.completions,
            report.cancellationRequests, report.cancellationAcknowledgements,
            report.outstandingAtEnd, report.readyCompletionsAtEnd,
            report.bytesRequested, report.bytesCompleted);
    } else job->status = UMI_STATUS_INVALID_ARGUMENT;
    if (job->status == UMI_STATUS_OK && (written < 0 || (size_t)written >= sizeof(job->report)))
        job->status = UMI_STATUS_CAPACITY_EXCEEDED;
}
static void InspectionWorker(GTask *task, gpointer source, gpointer data, GCancellable *cancel)
{
    (void)source; (void)cancel;
    EducationInspectionJob *job = data;
    /* The original four-format bounds are retained for review. The active table also bounds device I/O records. */
#if 0
    const size_t limits[] = {UMI_EDUCATION_SCHEDULE_MAX_FILE_BYTES, UMI_EDUCATION_TIMER_MAX_FILE_BYTES,
        UMI_EDUCATION_INTERRUPT_MAX_FILE_BYTES, UMI_EDUCATION_EVIDENCE_MAX_FILE_BYTES};
#endif
    const size_t limits[] = {UMI_EDUCATION_SCHEDULE_MAX_FILE_BYTES, UMI_EDUCATION_TIMER_MAX_FILE_BYTES,
        UMI_EDUCATION_INTERRUPT_MAX_FILE_BYTES, UMI_EDUCATION_EVIDENCE_MAX_FILE_BYTES, UMI_EDUCATION_DEVICE_IO_MAX_FILE_BYTES};
    unsigned char *bytes = NULL; size_t length = 0U;
    job->status = job->format < G_N_ELEMENTS(limits) ?
        UmiInputFileRead(job->path, limits[job->format], &bytes, &length) : UMI_STATUS_INVALID_ARGUMENT;
    if (job->status == UMI_STATUS_OK) InspectBytes(job, bytes, length);
    UmiInputFileFree(bytes);
    g_task_return_boolean(task, TRUE);
}
static void InspectionComplete(GObject *source, GAsyncResult *result, gpointer data)
{
    (void)source; (void)data;
    EducationInspectionJob *job = g_task_get_task_data(G_TASK(result));
    (void)g_task_propagate_boolean(G_TASK(result), NULL);
    GtkWidget *root = g_weak_ref_get(&job->root);
    if (root == NULL) return;
    EducationInspection *panel = g_object_get_data(G_OBJECT(root), "umicom-education-inspection");
    panel->busy = false;
    if (!panel->closed) {
        gtk_widget_set_sensitive(panel->inspect, TRUE); gtk_widget_set_sensitive(panel->discard, FALSE);
        bool unchanged = job->format == gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->format)) &&
            strcmp(job->path, gtk_editable_get_text(GTK_EDITABLE(panel->path))) == 0;
        if (panel->discardResult || !unchanged) {
            gtk_label_set_text(GTK_LABEL(panel->status), "Result discarded. Check the selected file and format, then inspect again.");
        } else if (job->status == UMI_STATUS_OK) {
            char *text = g_strdup_printf("Inspected file: %s\n\n%s\n\nTeaching simulation only. No Kernel task, device or interrupt was controlled.", job->path, job->report);
            gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->output)), text, -1); g_free(text);
            gtk_label_set_text(GTK_LABEL(panel->status), "Inspection complete. The report describes this teaching file only.");
        } else {
            char *message = g_strdup_printf("Inspection failed (status %d). Check the file, format, permissions and size limit. Any previous report remains labelled with its original file.", (int)job->status);
            gtk_label_set_text(GTK_LABEL(panel->status), message); g_free(message);
        }
    }
    g_object_unref(root);
}
static void InspectSelected(GtkButton *button, gpointer data)
{
    (void)button; EducationInspection *panel = data;
    if (panel->closed || panel->busy) return;
    const char *path = gtk_editable_get_text(GTK_EDITABLE(panel->path));
    guint format = gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->format));
    /* The original selector guard is retained for review; the active guard admits the additional device format. */
#if 0
    if (!g_path_is_absolute(path) || format > 3U) {
        gtk_label_set_text(GTK_LABEL(panel->status), "Choose an absolute local path and its teaching format."); return;
    }
#endif
    if (!g_path_is_absolute(path) || format > 4U) {
        gtk_label_set_text(GTK_LABEL(panel->status), "Choose an absolute local path and its teaching format."); return;
    }
    EducationInspectionJob *job = g_new0(EducationInspectionJob, 1);
    job->path = g_strdup(path); job->format = format; g_weak_ref_init(&job->root, panel->root);
    panel->busy = true; panel->discardResult = false;
    gtk_widget_set_sensitive(panel->inspect, FALSE); gtk_widget_set_sensitive(panel->discard, TRUE);
    gtk_label_set_text(GTK_LABEL(panel->status), "Reading the selected teaching file...");
    /* A worker owns only immutable path/format input and a weak widget reference.
     * Closing the host cannot leave it with a borrowed learner or IDE session. */
    GTask *task = g_task_new(NULL, NULL, InspectionComplete, NULL);
    g_task_set_task_data(task, job, InspectionJobFree); g_task_run_in_thread(task, InspectionWorker); g_object_unref(task);
}
static void DiscardInspection(GtkButton *button, gpointer data)
{
    (void)button; EducationInspection *panel = data;
    if (panel->closed || !panel->busy) return;
    panel->discardResult = true;
    gtk_label_set_text(GTK_LABEL(panel->status), "Result will be discarded. The accepted file read may still be finishing.");
}
static void InspectionFree(gpointer data)
{
    EducationInspection *panel = data;
    g_signal_handlers_disconnect_by_data(panel->inspect, panel);
    g_signal_handlers_disconnect_by_data(panel->discard, panel);
    UmiTextExportGtk4Retire(panel->textExport); g_clear_object(&panel->textExport);
    g_object_unref(panel->inspect); g_object_unref(panel->discard); g_free(panel);
}
GtkWidget *UmiEducationInspectionGtkCreate(void)
{
    EducationInspection *panel = g_new0(EducationInspection, 1);
    panel->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    g_object_set_data_full(G_OBJECT(panel->root), "umicom-education-inspection", panel, InspectionFree);
    InspectionTag(panel->root, "education.inspection");
    GtkWidget *label = gtk_label_new("Inspect a classroom trace without executing it. Select the format used by its producer.");
    gtk_label_set_wrap(GTK_LABEL(label), TRUE); gtk_box_append(GTK_BOX(panel->root), label);
    /* Keep the original menu for review. The active menu preserves its ordering and appends device I/O inspection. */
#if 0
    const char *formats[] = {"Scheduler trace", "Timer trace", "Interrupt trace", "Driver evidence trace", NULL};
#endif
    const char *formats[] = {"Scheduler trace", "Timer trace", "Interrupt trace", "Driver evidence trace", "Device I/O trace", NULL};
    panel->format = gtk_drop_down_new_from_strings(formats); InspectionTag(panel->format, "education.inspection.format");
    gtk_box_append(GTK_BOX(panel->root), panel->format);
    panel->path = gtk_entry_new(); InspectionTag(panel->path, "education.inspection.path");
    gtk_entry_set_placeholder_text(GTK_ENTRY(panel->path), "Absolute path to a teaching file");
    gtk_box_append(GTK_BOX(panel->root), panel->path);
    gtk_box_append(GTK_BOX(panel->root), UmiPathPickerGtk4Create(GTK_ENTRY(panel->path), UMI_PATH_PICKER_EXISTING_FILE, "Choose a teaching trace", NULL));
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6); gtk_box_append(GTK_BOX(panel->root), row);
    panel->inspect = gtk_button_new_with_label("Inspect selected file"); panel->discard = gtk_button_new_with_label("Discard pending result");
    g_object_ref(panel->inspect); g_object_ref(panel->discard);
    InspectionTag(panel->inspect, "education.inspection.read"); InspectionTag(panel->discard, "education.inspection.discard");
    g_signal_connect(panel->inspect, "clicked", G_CALLBACK(InspectSelected), panel);
    g_signal_connect(panel->discard, "clicked", G_CALLBACK(DiscardInspection), panel);
    gtk_box_append(GTK_BOX(row), panel->inspect); gtk_box_append(GTK_BOX(row), panel->discard);
    gtk_widget_set_sensitive(panel->discard, FALSE);
    panel->status = gtk_label_new("No file has been inspected."); gtk_label_set_wrap(GTK_LABEL(panel->status), TRUE);
    InspectionTag(panel->status, "education.inspection.status"); gtk_box_append(GTK_BOX(panel->root), panel->status);
    panel->output = gtk_text_view_new(); gtk_text_view_set_editable(GTK_TEXT_VIEW(panel->output), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(panel->output), GTK_WRAP_WORD_CHAR);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(panel->output), TRUE); InspectionTag(panel->output, "education.inspection.report");
    GtkWidget *scroll = gtk_scrolled_window_new(); gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), panel->output);
    gtk_widget_set_size_request(scroll, -1, 220); gtk_box_append(GTK_BOX(panel->root), scroll);
    /* Save precisely the labelled inspection on screen, independent of edits to
     * the next file selection. The common exporter owns all disk work. */
    panel->textExport = UmiTextExportGtk4Create(panel->output, "teaching-inspection.txt");
    g_object_ref_sink(panel->textExport);
    gtk_box_append(GTK_BOX(panel->root), panel->textExport);
    return panel->root;
}
void UmiEducationInspectionGtkRetire(GtkWidget *widget)
{
    if (widget == NULL) return;
    EducationInspection *panel = g_object_get_data(G_OBJECT(widget), "umicom-education-inspection");
    if (panel == NULL || panel->closed) return;
    panel->closed = true;
    UmiTextExportGtk4Retire(panel->textExport);
    g_signal_handlers_disconnect_by_data(panel->inspect, panel); g_signal_handlers_disconnect_by_data(panel->discard, panel);
    gtk_widget_set_sensitive(widget, FALSE);
}

bool UmiEducationInspectionGtkBusy(GtkWidget *widget)
{
    if (widget == NULL) return false;
    const EducationInspection *panel = g_object_get_data(G_OBJECT(widget), "umicom-education-inspection");
    return panel != NULL && panel->busy;
}
