/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/education_project_export_gtk4.c
 * PURPOSE: Keep lesson export responsive and bind its result to the accepted path.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "education_workspace_private.h"
#include "umicom/developer_project/new_project.h"
#include "umicom/ui/gtk4/path_picker.h"
#include <string.h>

typedef struct EducationProjectExport {
    char *projectId, *destination;
    UmiEducationProjectWorkflow workflow;
    UmiStatus status;
    size_t written;
} EducationProjectExport;

static void ExportFree(gpointer data)
{
    /* Task data contains no GTK objects or database owners. GLib can release it
     * on either thread after the completion callback has consumed the result. */
    EducationProjectExport *request = data;
    g_free(request->projectId);
    g_free(request->destination);
    g_free(request);
}
static void ExportWorker(GTask *task, gpointer source, gpointer data, GCancellable *cancel)
{
    (void)source;
    (void)cancel;
    EducationProjectExport *request = data;
    /* Only canonical immutable course resources and copied request data cross
     * this boundary. The worker never consults widgets, learner data or an IDE. */
    request->status = UmiEducationProjectExportForDevelopment(request->projectId,
        request->destination, &request->written, &request->workflow);
    g_task_return_boolean(task, request->status == UMI_STATUS_OK);
}
static void ExportComplete(GObject *source, GAsyncResult *result, gpointer data)
{
    (void)source;
    UmiEducationGtkPanel *panel = data;
    GTask *task = G_TASK(result);
    (void)g_task_propagate_boolean(task, NULL);
    EducationProjectExport *request = g_task_get_task_data(task);
    /* Destroy disconnects controls immediately. Completion still releases the
     * held controller on the GTK thread, but never revives a closed window. */
    if (!panel->closed) {
        if (request->status == UMI_STATUS_OK) {
            *panel->exportedProject = request->workflow;
            panel->exportedProjectReady = true;
            gtk_label_set_text(panel->exportedProjectLabel, request->destination);
        } else {
            char *message = g_strdup_printf(
                "Export did not complete at %s. %zu files were fully written. "
                "Keep any partial new folder for inspection.",
                request->destination, request->written);
            gtk_label_set_text(panel->exportedProjectLabel, message);
            g_free(message);
        }
        if (!panel->closed) {
            char *message = g_strdup_printf(
                "Exported %zu files to %s. Review the project, then open it in the IDE. "
                "No compiler was started.%s", request->written, request->destination,
                request->workflow.requires_framework_sdk
                    ? " This course needs an installed Framework data SDK." : "");
            EwGtkStatus(panel, request->status, message);
            g_free(message);
        }
    }
    /* Keep the request marked pending during result notifications. A reentrant
     * click cannot replace the result while it is still being published. */
    panel->projectExporting = false;
    if (!panel->closed)
        gtk_widget_set_sensitive(panel->exportProjectButton, TRUE);
    if (!panel->closed)
        gtk_widget_set_sensitive(panel->openProjectButton,
            panel->exportedProjectReady && panel->openProject != NULL);
    EwGtkRelease(panel);
}
void EwGtkExportCourseProject(UmiEducationGtkPanel *panel, const UmiEducationLesson *lesson)
{
    if (panel->closed || panel->projectExporting || lesson == NULL)
        return;
    EducationProjectExport *request = g_try_new0(EducationProjectExport, 1U);
    if (request == NULL) {
        EwGtkStatus(panel, UMI_STATUS_OUT_OF_MEMORY, "");
        return;
    }
    /* Capture before changing widget properties: their notifications can edit
     * the draft path or select another lesson. Such edits belong to a later job. */
    request->projectId = g_strdup(lesson->projectId);
    request->destination = g_strdup(gtk_editable_get_text(GTK_EDITABLE(panel->projectPath)));
    request->status = UmiEducationProjectWorkflowDescribe(request->projectId,
        request->destination, &request->workflow);
    if (panel->exportedProject == NULL)
        panel->exportedProject = g_try_new0(UmiEducationProjectWorkflow, 1U);
    if (panel->exportedProject == NULL)
        request->status = UMI_STATUS_OUT_OF_MEMORY;
    panel->projectExporting = true;
    panel->exportedProjectReady = false;
    gtk_widget_set_sensitive(panel->openProjectButton, FALSE);
    if (!panel->closed)
        gtk_widget_set_sensitive(panel->exportProjectButton, FALSE);
    if (panel->closed || request->status != UMI_STATUS_OK) {
        panel->projectExporting = false;
        if (!panel->closed) {
            gtk_label_set_text(panel->exportedProjectLabel,
                "Export did not start. Check the absolute path to a new project folder.");
            if (!panel->closed)
                EwGtkStatus(panel, request->status, "");
            if (!panel->closed)
                gtk_widget_set_sensitive(panel->exportProjectButton, TRUE);
        }
        ExportFree(request);
        return;
    }
    gtk_label_set_text(panel->exportedProjectLabel, request->destination);
    if (!panel->closed)
        EwGtkMessage(panel, "Exporting the selected course. You can keep reading. "
            "Closing this window does not undo files already accepted for export.");
    if (panel->closed) {
        panel->projectExporting = false;
        ExportFree(request);
        return;
    }
    /* The callback owns this reference, not the worker. There is deliberately
     * no cancellation promise once filesystem work begins: exclusive writes
     * preserve existing destinations and any partial new export stays visible. */
    GTask *task = g_task_new(NULL, NULL, ExportComplete, EwGtkAcquire(panel));
    g_task_set_task_data(task, request, ExportFree);
    g_task_run_in_thread(task, ExportWorker);
    g_object_unref(task);
}
static GtkEntry *DestinationEntry(GtkWidget *box, const char *label, const char *id)
{
    GtkWidget *caption = gtk_label_new(label);
    gtk_label_set_xalign(GTK_LABEL(caption), 0.0F);
    gtk_box_append(GTK_BOX(box), caption);
    GtkWidget *entry = gtk_entry_new();
    g_object_set_data_full(G_OBJECT(entry), "umicom-automation-id", g_strdup(id), g_free);
    gtk_box_append(GTK_BOX(box), entry);
    return GTK_ENTRY(entry);
}
static void UseDestination(GtkButton *button, gpointer data)
{
    (void)button;
    UmiEducationGtkPanel *panel = EwGtkAcquire(data);
    if (panel == NULL)
        return;
    if (panel->actionRunning || panel->projectExporting) {
        EwGtkRelease(panel);
        return;
    }
    panel->actionRunning = true;
    char destination[1024];
    UmiStatus status = UmiDeveloperProjectChooseDestination(
        gtk_editable_get_text(GTK_EDITABLE(panel->projectParent)),
        gtk_editable_get_text(GTK_EDITABLE(panel->projectName)),
        destination, sizeof destination);
    /* Reuse Studio's portable path rules rather than a second path parser.
     * Preparing the destination performs no I/O and grants no execution trust. */
    if (status == UMI_STATUS_OK)
        gtk_editable_set_text(GTK_EDITABLE(panel->projectPath), destination);
    if (!panel->closed)
        EwGtkStatus(panel, status, "Project destination prepared. Review it above, "
            "then choose Export this course project. No folder was created.");
    panel->actionRunning = false;
    EwGtkRelease(panel);
}
void EwGtkProjectDestinationControls(UmiEducationGtkPanel *panel)
{
    GtkWidget *section = gtk_expander_new("Choose a parent folder and a new project name");
    GtkWidget *body = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    panel->projectParent = DestinationEntry(body, "Existing parent folder", "education.project-parent");
    gtk_box_append(GTK_BOX(body), UmiPathPickerGtk4Create(panel->projectParent,
        UMI_PATH_PICKER_DIRECTORY, "Choose the parent for your new course project", NULL));
    panel->projectName = DestinationEntry(body, "New folder name", "education.project-name");
    GtkWidget *use = gtk_button_new_with_label("Use this project destination");
    g_object_set_data_full(G_OBJECT(use), "umicom-automation-id",
        g_strdup("education.project-destination"), g_free);
    g_signal_connect(use, "clicked", G_CALLBACK(UseDestination), panel);
    gtk_box_append(GTK_BOX(body), use);
    gtk_expander_set_child(GTK_EXPANDER(section), body);
    gtk_box_append(GTK_BOX(panel->root), section);
}
