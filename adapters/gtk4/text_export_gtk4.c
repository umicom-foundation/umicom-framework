/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/text_export_gtk4.c
 * PURPOSE: Give product reports one bounded snapshot and file-writing owner.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/text_export.h"
#include "umicom/ui/gtk4/path_picker.h"
#include "umicom/platform/output_file.h"
#include <string.h>

typedef struct TextExport {
    GtkWidget *root, *path, *save, *status;
    GWeakRef source;
    bool busy, retired;
    UmiTextExportAdmission admission;
    void *admission_context;
    GDestroyNotify admission_destroy;
} TextExport;

typedef struct TextExportWrite {
    GWeakRef root;
    char *path, *text;
    size_t length;
    UmiStatus status;
    uint64_t written;
} TextExportWrite;

static void ExportTag(GtkWidget *widget, const char *name)
{
    g_object_set_data_full(G_OBJECT(widget), "umicom-automation-id", g_strdup(name), g_free);
}

static void ExportWriteFree(gpointer data)
{
    TextExportWrite *write = data;
    g_weak_ref_clear(&write->root);
    g_free(write->path);
    g_free(write->text);
    g_free(write);
}

static UmiStatus ExportCapture(GObject *source, char **text, size_t *length)
{
    /* Read widget state only on the GTK thread. The character bound limits the
     * temporary UTF-8 copy; the byte bound is checked before accepting a job. */
    *text = NULL;
    if (GTK_IS_TEXT_VIEW(source) || GTK_IS_TEXT_BUFFER(source)) {
        GtkTextBuffer *buffer = GTK_IS_TEXT_BUFFER(source) ? GTK_TEXT_BUFFER(source) :
            gtk_text_view_get_buffer(GTK_TEXT_VIEW(source));
        if (gtk_text_buffer_get_char_count(buffer) > UMI_TEXT_EXPORT_MAX_CHARACTERS)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        GtkTextIter start, end;
        gtk_text_buffer_get_bounds(buffer, &start, &end);
        *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
    } else if (GTK_IS_LABEL(source)) {
        const char *label = gtk_label_get_text(GTK_LABEL(source));
        size_t bytes = strlen(label);
        if (bytes > UMI_TEXT_EXPORT_MAX_BYTES ||
            g_utf8_strlen(label, (gssize)bytes) > UMI_TEXT_EXPORT_MAX_CHARACTERS)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        *text = g_strdup(label);
    } else return UMI_STATUS_INVALID_STATE;
    *length = strlen(*text);
    if (*length == 0U || *length > UMI_TEXT_EXPORT_MAX_BYTES) {
        g_clear_pointer(text, g_free);
        return *length == 0U ? UMI_STATUS_INVALID_STATE : UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

static void ExportWriteFile(TextExportWrite *write)
{
    /* The platform owner performs exclusive creation and remembers write errors.
     * Close is always attempted, but must not hide an earlier write failure. */
    UmiOutputFile *file = NULL;
    write->status = UmiOutputFileCreate(write->path, &file);
    if (write->status == UMI_STATUS_OK) {
        write->status = UmiOutputFileWrite(file, write->text, write->length);
        UmiStatus closed = UmiOutputFileClose(file);
        if (write->status == UMI_STATUS_OK) write->status = closed;
        UmiOutputFileSnapshot snapshot = {0};
        if (UmiOutputFileRead(file, &snapshot) == UMI_STATUS_OK)
            write->written = snapshot.bytes_written;
    }
    UmiOutputFileDestroy(file);
}

static void ExportWorker(GTask *task, gpointer source, gpointer data, GCancellable *cancel)
{
    (void)source; (void)cancel;
    ExportWriteFile(data);
    g_task_return_boolean(task, TRUE);
}

/* Keep completion guarded through observable status/sensitivity changes.
 * The earlier release-before-feedback path is retained for review. */
#if 0
static void ExportComplete(GObject *source, GAsyncResult *result, gpointer data)
{
    (void)source; (void)data;
    TextExportWrite *write = g_task_get_task_data(G_TASK(result));
    (void)g_task_propagate_boolean(G_TASK(result), NULL);
    GtkWidget *root = g_weak_ref_get(&write->root);
    if (root == NULL) return;
    TextExport *export = g_object_get_data(G_OBJECT(root), "umicom-text-export");
    export->busy = false;
    if (!export->retired) {
        /* The receipt names the accepted destination, even if the entry or report
         * changed while writing. It never describes a newer report as saved. */
        char *message = write->status == UMI_STATUS_OK ?
            g_strdup_printf("Saved captured text (%" G_GUINT64_FORMAT " bytes) to %s",
                (guint64)write->written, write->path) :
            g_strdup_printf("Save failed (status %d; %" G_GUINT64_FORMAT
                " bytes written) at %s. Existing files are preserved. A new partial file may remain; inspect it before choosing another filename.",
                (int)write->status, (guint64)write->written, write->path);
        gtk_label_set_text(GTK_LABEL(export->status), message);
        g_free(message);
        if (!export->retired) gtk_widget_set_sensitive(export->save, TRUE);
    }
    g_object_unref(root);
}

#endif
static void ExportComplete(GObject *source, GAsyncResult *result, gpointer data)
{
    (void)source; (void)data;
    TextExportWrite *write = g_task_get_task_data(G_TASK(result));
    (void)g_task_propagate_boolean(G_TASK(result), NULL);
    GtkWidget *root = g_weak_ref_get(&write->root);
    if (root == NULL) return;
    TextExport *export = g_object_get_data(G_OBJECT(root), "umicom-text-export");
    if (!export->retired) {
        /* The receipt names the accepted destination, even if the entry or report
         * changed while writing. It never describes a newer report as saved. */
        char *message = write->status == UMI_STATUS_OK ?
            g_strdup_printf("Saved captured text (%" G_GUINT64_FORMAT " bytes) to %s",
                (guint64)write->written, write->path) :
            g_strdup_printf("Save failed (status %d; %" G_GUINT64_FORMAT
                " bytes written) at %s. Existing files are preserved. A new partial file may remain; inspect it before choosing another filename.",
                (int)write->status, (guint64)write->written, write->path);
        gtk_label_set_text(GTK_LABEL(export->status), message);
        g_free(message);
        if (!export->retired) gtk_widget_set_sensitive(export->save, TRUE);
    }
    export->busy = false;
    g_object_unref(root);
}

/* Preserve direct capture for review. The replacement owns admission before
 * owner callbacks and refusal feedback, preventing reentrant writes. */
#if 0
static void ExportClicked(GtkButton *button, gpointer data)
{
    (void)button;
    GtkWidget *root = g_object_ref(data);
    TextExport *export = g_object_get_data(G_OBJECT(root), "umicom-text-export");
    if (export->retired || export->busy) { g_object_unref(root); return; }
    const char *path = gtk_editable_get_text(GTK_EDITABLE(export->path));
    UmiStatus status = UmiOutputFileValidatePath(path);
    TextExportWrite *write = g_new0(TextExportWrite, 1);
    g_weak_ref_init(&write->root, root);
    write->path = g_strdup(path);
    GObject *source = g_weak_ref_get(&export->source);
    if (status == UMI_STATUS_OK) status = ExportCapture(source, &write->text, &write->length);
    g_clear_object(&source);
    if (status != UMI_STATUS_OK) {
        char *message = g_strdup_printf("Cannot capture text (status %d). Choose a new absolute filename and a nonempty report within the export size limit. No file was created.", (int)status);
        gtk_label_set_text(GTK_LABEL(export->status), message);
        g_free(message);
        ExportWriteFree(write);
        g_object_unref(root);
        return;
    }
    /* Set busy before signals can re-enter this control. Job data has no GTK or
     * application-service ownership; final widget references stay on GTK's thread. */
    export->busy = true;
    GTask *task = g_task_new(NULL, NULL, ExportComplete, NULL);
    g_task_set_task_data(task, write, ExportWriteFree);
    g_task_run_in_thread(task, ExportWorker);
    g_object_unref(task);
    gtk_widget_set_sensitive(export->save, FALSE);
    if (!export->retired)
        gtk_label_set_text(GTK_LABEL(export->status), "Saving the captured text. Later report edits are not part of this file.");
    g_object_unref(root);
}

#endif
static void ExportClicked(GtkButton *button, gpointer data)
{
    (void)button;
    GtkWidget *root=g_object_ref(data);
    TextExport *export=g_object_get_data(G_OBJECT(root),"umicom-text-export");
    if(export->retired||export->busy){g_object_unref(root);return;}
    /* A caller-supplied admission callback and every feedback notification may
     * reenter this control. Hold admission until the worker or refusal owns the
     * complete result; the worker receives only copied text and destination. */
    export->busy=true;
    TextExportWrite *write=g_new0(TextExportWrite,1);
    g_weak_ref_init(&write->root,root);
    write->path=g_strdup(gtk_editable_get_text(GTK_EDITABLE(export->path)));
    UmiStatus status=UmiOutputFileValidatePath(write->path);
    if(status==UMI_STATUS_OK&&export->admission!=NULL)
        status=export->admission(export->admission_context);
    if(export->retired)status=UMI_STATUS_CANCELLED;
    GObject *source=g_weak_ref_get(&export->source);
    if(status==UMI_STATUS_OK)status=ExportCapture(source,&write->text,&write->length);
    g_clear_object(&source);
    if(export->retired)status=UMI_STATUS_CANCELLED;
    if(status!=UMI_STATUS_OK){
        if(!export->retired){
            char *message=g_strdup_printf("Cannot capture text (status %d). Review the report's current source and regenerate it if needed. Choose a new absolute filename and a nonempty report within the export size limit. No file was created.",(int)status);
            gtk_label_set_text(GTK_LABEL(export->status),message);g_free(message);
        }
        export->busy=false;ExportWriteFree(write);g_object_unref(root);return;
    }
    GTask *task=g_task_new(NULL,NULL,ExportComplete,NULL);
    g_task_set_task_data(task,write,ExportWriteFree);
    g_task_run_in_thread(task,ExportWorker);g_object_unref(task);
    gtk_widget_set_sensitive(export->save,FALSE);
    if(!export->retired)gtk_label_set_text(GTK_LABEL(export->status),
        "Saving the captured text. Later report edits are not part of this file.");
    g_object_unref(root);
}

static void ExportFree(gpointer data)
{
    TextExport *export = data;
    g_signal_handlers_disconnect_by_data(export->save, export->root);
    g_weak_ref_clear(&export->source);
    g_object_unref(export->save);
    g_object_unref(export->path); g_object_unref(export->status);
    if(export->admission_destroy!=NULL)export->admission_destroy(export->admission_context);
    g_free(export);
}

static GtkWidget *ExportCreate(GObject *source, const char *suggestedFilename)
{
    if ((!GTK_IS_LABEL(source) && !GTK_IS_TEXT_VIEW(source) && !GTK_IS_TEXT_BUFFER(source)) || suggestedFilename == NULL ||
        suggestedFilename[0] == '\0' || !g_utf8_validate(suggestedFilename, -1, NULL)) return NULL;
    TextExport *export = g_new0(TextExport, 1);
    /* A collapsed section leaves room for the report in smaller workspaces.
     * Expanding it is presentation only; it does not create or write a file. */
    export->root = gtk_expander_new("Save displayed text");
    GtkWidget *body = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_expander_set_child(GTK_EXPANDER(export->root), body);
    g_weak_ref_init(&export->source, source);
    g_object_set_data_full(G_OBJECT(export->root), "umicom-text-export", export, ExportFree);
    ExportTag(export->root, "text.export");
    export->path = gtk_entry_new();
    g_object_ref(export->path);
    gtk_entry_set_max_length(GTK_ENTRY(export->path), (int)UMI_PATH_CAPACITY - 1);
    gtk_entry_set_placeholder_text(GTK_ENTRY(export->path), "New absolute text filename");
    ExportTag(export->path, "text.export.path");
    gtk_accessible_update_property(GTK_ACCESSIBLE(export->path), GTK_ACCESSIBLE_PROPERTY_LABEL,
        "Save displayed text to a new file", -1);
    gtk_box_append(GTK_BOX(body), export->path);
    gtk_box_append(GTK_BOX(body), UmiPathPickerGtk4Create(GTK_ENTRY(export->path),
        UMI_PATH_PICKER_NEW_FILE, "Save displayed text", suggestedFilename));
    export->save = gtk_button_new_with_label("Save displayed text");
    g_object_ref(export->save);
    ExportTag(export->save, "text.export.save");
    g_signal_connect_object(export->save, "clicked", G_CALLBACK(ExportClicked), export->root, 0);
    gtk_box_append(GTK_BOX(body), export->save);
    export->status = gtk_label_new("Saves a copy of the displayed text. Existing files are kept; choose a folder you control.");
    g_object_ref(export->status);
    gtk_label_set_wrap(GTK_LABEL(export->status), TRUE);
    gtk_label_set_selectable(GTK_LABEL(export->status), TRUE);
    gtk_label_set_xalign(GTK_LABEL(export->status), 0.0F);
    ExportTag(export->status, "text.export.status");
    gtk_box_append(GTK_BOX(body), export->status);
    return export->root;
}

GtkWidget *UmiTextExportGtk4Create(GtkWidget *source, const char *suggestedFilename)
{
    if (!GTK_IS_LABEL(source) && !GTK_IS_TEXT_VIEW(source)) return NULL;
    return ExportCreate(G_OBJECT(source), suggestedFilename);
}

GtkWidget *UmiTextExportGtk4CreateForBuffer(GtkTextBuffer *source, const char *suggestedFilename)
{
    if (!GTK_IS_TEXT_BUFFER(source)) return NULL;
    return ExportCreate(G_OBJECT(source), suggestedFilename);
}

/* Retained children can leave their original container. Retire their own
 * controls as well as inherited sensitivity, and hold the composite through
 * synchronous notifications that may release its last application reference.
 * Keep the container-only implementation for review of the original contract. */
#if 0
void UmiTextExportGtk4Retire(GtkWidget *widget)
{
    if (widget == NULL) return;
    TextExport *export = g_object_get_data(G_OBJECT(widget), "umicom-text-export");
    if (export == NULL || export->retired) return;
    export->retired = true;
    g_weak_ref_set(&export->source, NULL);
    g_signal_handlers_disconnect_by_data(export->save, export->root);
    gtk_widget_set_sensitive(widget, FALSE);
}

#endif
void UmiTextExportGtk4Retire(GtkWidget *widget)
{
    if (widget == NULL) return;
    TextExport *export = g_object_get_data(G_OBJECT(widget), "umicom-text-export");
    if (export == NULL || export->retired) return;
    g_object_ref(widget);
    export->retired = true;
    g_weak_ref_set(&export->source, NULL);
    g_signal_handlers_disconnect_by_data(export->save, export->root);
    gtk_widget_set_sensitive(export->save, FALSE);
    gtk_widget_set_sensitive(export->path, FALSE);
    gtk_widget_set_sensitive(widget, FALSE);
    g_object_unref(widget);
}

bool UmiTextExportGtk4Busy(GtkWidget *widget)
{
    if (widget == NULL) return false;
    TextExport *export = g_object_get_data(G_OBJECT(widget), "umicom-text-export");
    return export != NULL && export->busy;
}

GtkWidget *UmiTextExportGtk4CreateGuardedBuffer(GtkTextBuffer *source,
    const char *suggestedFilename,UmiTextExportAdmission admission,
    void *context,GDestroyNotify destroy_context)
{
    if(!GTK_IS_TEXT_BUFFER(source)||admission==NULL)return NULL;
    GtkWidget *root=ExportCreate(G_OBJECT(source),suggestedFilename);
    if(root!=NULL){
        TextExport *export=g_object_get_data(G_OBJECT(root),"umicom-text-export");
        export->admission=admission;export->admission_context=context;
        export->admission_destroy=destroy_context;
    }
    return root;
}
