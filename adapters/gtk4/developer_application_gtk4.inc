/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/developer_application_gtk4.inc
 * PURPOSE: Review and create native application source with owned background work.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


struct DeveloperApplicationForm {
    UmiApplicationScaffoldPlan *plan;
    GtkWidget *preview, *files, *text, *gtk, *choose;
    int busy, applying, invalidated, changing;
    uint64_t editRevision;
    int revisionExhausted;
};

/* A task owns every input string and its plan. It never reads a GTK object from
 * the worker thread. The completion owns one dialog reference, so teardown can
 * retire callbacks without freeing memory while a write is still in progress. */
typedef struct DeveloperApplicationJob {
    char fields[5][UMI_PATH_CAPACITY];
    bool gtk;
    int apply;
    UmiStatus status;
    UmiApplicationScaffoldPlan *plan;
    UmiApplicationScaffoldSummary summary;
    UmiApplicationScaffoldResult result;
    UmiDeveloperProjectModel model;
    UmiBuildProfile profile;
} DeveloperApplicationJob;

static void DeveloperApplicationDispose(DeveloperApplicationForm *form)
{
    if (form == NULL) return;
    umi_application_scaffold_destroy(form->plan);
    free(form);
}

static int DeveloperApplicationMayClose(UmiGtk4DeveloperDialog *dialog)
{
    if (dialog->application == NULL || !dialog->application->applying) return 1;
    ShowStatus(dialog, UMI_STATUS_BUSY,
        "Source creation is finishing. Wait for its result before closing. Files already written are retained.");
    return 0;
}

/* Sensitivity helps the user follow the workflow; every callback independently
 * checks the same state because retained widgets can still emit their signals. */
static void DeveloperApplicationControls(UmiGtk4DeveloperDialog *dialog)
{
    DeveloperApplicationForm *form = dialog->application;
    int editable = !form->applying && !dialog->generated;
    for (size_t i = 0U; i < 5U; ++i)
        gtk_widget_set_sensitive(dialog->fields[i], editable);
    gtk_widget_set_sensitive(form->gtk, editable);
    gtk_widget_set_sensitive(form->choose, editable && !form->busy && !dialog->projectChoosing);
    gtk_widget_set_sensitive(form->preview, editable && !form->busy && !dialog->projectChoosing && !form->revisionExhausted);
    gtk_widget_set_sensitive(dialog->acceptButton, !form->busy && !dialog->projectChoosing &&
        (dialog->generated || (form->plan != NULL && !form->invalidated && !form->revisionExhausted)));
}

/* Clear both the owned source and its visual projection. Changing a field back
 * to its former text still requires another review; equality is not consent. */
static void DeveloperApplicationInvalidate(UmiGtk4DeveloperDialog *dialog)
{
    DeveloperApplicationForm *form = dialog->application;
    form->invalidated = 1;
    umi_application_scaffold_destroy(form->plan);
    form->plan = NULL;
    form->changing = 1;
    gtk_drop_down_set_model(GTK_DROP_DOWN(form->files), NULL);
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(form->text)), "", -1);
    form->changing = 0;
}

static void DeveloperApplicationEdited(GtkEditable *entry, gpointer context)
{
    (void)entry;
    UmiGtk4DeveloperDialog *dialog = DeveloperAcquire(context);
    if (dialog == NULL) return;
    DeveloperApplicationForm *form = dialog->application;
    if (!form->applying && !dialog->generated) {
        if (form->editRevision == UINT64_MAX) form->revisionExhausted = 1;
        else ++form->editRevision;
        DeveloperApplicationInvalidate(dialog);
        DeveloperApplicationControls(dialog);
        if (DeveloperVisible(dialog)) ShowStatus(dialog, UMI_STATUS_OK,
            "Inputs changed. Preview the source again before creating files.");
    }
    DeveloperRelease(dialog);
}

static void DeveloperApplicationToggle(GtkCheckButton *button, gpointer context)
{
    (void)button;
    DeveloperApplicationEdited(NULL, context);
}

static void DeveloperApplicationFileSelected(GObject *object, GParamSpec *property, gpointer context)
{
    (void)object; (void)property;
    UmiGtk4DeveloperDialog *dialog = DeveloperAcquire(context);
    if (dialog == NULL) return;
    DeveloperApplicationForm *form = dialog->application;
    if (!form->changing && form->plan != NULL && !form->invalidated && !form->busy) {
        UmiApplicationScaffoldFile file;
        guint index = gtk_drop_down_get_selected(GTK_DROP_DOWN(form->files));
        UmiStatus status = umi_application_scaffold_file(form->plan, (size_t)index, &file);
        if (status == UMI_STATUS_OK)
            gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(form->text)),
                file.text, (int)file.length);
    }
    DeveloperRelease(dialog);
}

static void DeveloperApplicationJobFree(gpointer context)
{
    DeveloperApplicationJob *job = context;
    umi_application_scaffold_destroy(job->plan);
    free(job);
}

/* Preparing adoption settings before writes catches an identifier that fits the
 * manifest but exceeds the IDE model. No partially created project is needed to
 * discover that error. Apply uses the already displayed byte snapshot. */
static void DeveloperApplicationWorker(GTask *task, gpointer source, gpointer data, GCancellable *cancel)
{
    (void)source; (void)cancel;
    DeveloperApplicationJob *job = data;
    if (job->apply) {
        job->status = umi_application_scaffold_apply(job->plan, &job->result);
    } else {
        if (job->fields[3][0] == '\0')
            job->status = umi_application_scaffold_template_root(job->fields[3], sizeof job->fields[3]);
        else job->status = UMI_STATUS_OK;
        UmiApplicationScaffoldRequest request = {
            .template_root = job->fields[3], .destination_parent = job->fields[4],
            .slug = job->fields[2], .application_name = job->fields[0],
            .application_id = job->fields[1], .gtk_frontend = job->gtk
        };
        if (job->status == UMI_STATUS_OK)
            job->status = umi_application_scaffold_plan(&request, &job->plan);
        if (job->status == UMI_STATUS_OK)
            job->status = umi_application_scaffold_summary(job->plan, &job->summary);
        /* GTK displays UTF-8 text. Reject an unreadable custom template before
         * offering Create, while leaving the lower-level byte service reusable. */
        for (size_t i = 0U; job->status == UMI_STATUS_OK && i < job->summary.file_count; ++i) {
            UmiApplicationScaffoldFile file;
            job->status = umi_application_scaffold_file(job->plan, i, &file);
            if (job->status == UMI_STATUS_OK && !g_utf8_validate(file.text, (gssize)file.length, NULL))
                job->status = UMI_STATUS_PARSE_ERROR;
        }
        if (job->status == UMI_STATUS_OK)
            job->status = UmiDeveloperApplicationScaffoldProject(job->plan, &job->model, &job->profile);
    }
    g_task_return_boolean(task, TRUE);
}

/* The visible-only completion is retained for review. A write may finish
 * while its owned form is hidden; its result and Open Project state must still
 * be retained. Closed hosts remain excluded, and hidden controls cannot accept. */
#if 0
static void DeveloperApplicationCompleted(GObject *source, GAsyncResult *result, gpointer context)
{
    (void)source;
    UmiGtk4DeveloperDialog *dialog = context;
    DeveloperApplicationJob *job = g_task_get_task_data(G_TASK(result));
    DeveloperApplicationForm *form = dialog->application;
    (void)g_task_propagate_boolean(G_TASK(result), NULL);
    /* Keep the operation busy while GTK publishes its model and labels.
     * Notification handlers must not start another operation halfway through
     * adoption of this result. Controls are enabled only at the end. */
    if (DeveloperVisible(dialog)) {
        if (job->apply) {
            if (job->status == UMI_STATUS_OK) {
                /* The consumed plan can still provide read-only file views.
                 * Keep those bytes available while the user reviews adoption. */
                form->plan = job->plan; job->plan = NULL;
                dialog->generated = 1;
                dialog->model.generated = 1;
                gtk_button_set_label(GTK_BUTTON(dialog->acceptButton), "Open Project");
                ShowStatus(dialog, UMI_STATUS_OK,
                    "Source files created. Open Project to edit them, then review Build Settings and workspace trust.");
            } else {
                char message[UMI_PATH_CAPACITY + 256U];
                (void)g_snprintf(message, sizeof message,
                    "Creation stopped: %s. Completed %zu of %zu created files. Last path: %s. "
                    "Keep these files for inspection; choose a fresh destination to retry.",
                    umi_status_text(job->status), job->result.completed_files,
                    job->result.created_files, job->result.last_path);
                ShowStatus(dialog, job->status, message);
            }
        } else if (!form->invalidated) {
            if (job->status == UMI_STATUS_OK) {
                form->plan = job->plan; job->plan = NULL;
                dialog->model = job->model; dialog->profile = job->profile;
                GtkStringList *names = gtk_string_list_new(NULL);
                for (size_t i = 0U; i < job->summary.file_count; ++i) {
                    UmiApplicationScaffoldFile file;
                    if (umi_application_scaffold_file(form->plan, i, &file) == UMI_STATUS_OK)
                        gtk_string_list_append(names, file.relative_path);
                }
                form->changing = 1;
                gtk_drop_down_set_model(GTK_DROP_DOWN(form->files), G_LIST_MODEL(names));
                g_object_unref(names);
                gtk_drop_down_set_selected(GTK_DROP_DOWN(form->files), 0U);
                form->changing = 0;
                if (DeveloperVisible(dialog) && form->plan != NULL && !form->invalidated) {
                    UmiApplicationScaffoldFile first;
                    if (umi_application_scaffold_file(form->plan, 0U, &first) == UMI_STATUS_OK)
                        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(form->text)),
                            first.text, (int)first.length);
                }
                if (DeveloperVisible(dialog) && !form->invalidated) {
                    char message[UMI_PATH_CAPACITY + 256U];
                    (void)g_snprintf(message, sizeof message,
                        "Review %zu files (%zu bytes) for %s. Create Source writes this reviewed snapshot.",
                        job->summary.file_count, job->summary.byte_count, job->summary.root);
                    ShowStatus(dialog, UMI_STATUS_OK, message);
                }
            } else ShowStatus(dialog, job->status, NULL);
        }
    }
    form->busy = 0;
    form->applying = 0;
    if (DeveloperVisible(dialog)) DeveloperApplicationControls(dialog);
    DeveloperRelease(dialog);
}

#endif
static void DeveloperApplicationCompleted(GObject *source, GAsyncResult *result, gpointer context)
{
    (void)source;
    UmiGtk4DeveloperDialog *dialog = context;
    DeveloperApplicationJob *job = g_task_get_task_data(G_TASK(result));
    DeveloperApplicationForm *form = dialog->application;
    (void)g_task_propagate_boolean(G_TASK(result), NULL);
    /* Keep the operation busy while GTK publishes its model and labels.
     * Notification handlers must not start another operation halfway through
     * adoption of this result. Controls are enabled only at the end. */
    if (DeveloperAlive(dialog)) {
        if (job->apply) {
            if (job->status == UMI_STATUS_OK) {
                /* The consumed plan can still provide read-only file views.
                 * Keep those bytes available while the user reviews adoption. */
                form->plan = job->plan; job->plan = NULL;
                dialog->generated = 1;
                dialog->model.generated = 1;
                gtk_button_set_label(GTK_BUTTON(dialog->acceptButton), "Open Project");
                ShowStatus(dialog, UMI_STATUS_OK,
                    "Source files created. Open Project to edit them, then review Build Settings and workspace trust.");
            } else {
                char message[UMI_PATH_CAPACITY + 256U];
                (void)g_snprintf(message, sizeof message,
                    "Creation stopped: %s. Completed %zu of %zu created files. Last path: %s. "
                    "Keep these files for inspection; choose a fresh destination to retry.",
                    umi_status_text(job->status), job->result.completed_files,
                    job->result.created_files, job->result.last_path);
                ShowStatus(dialog, job->status, message);
            }
        } else if (!form->invalidated) {
            if (job->status == UMI_STATUS_OK) {
                form->plan = job->plan; job->plan = NULL;
                dialog->model = job->model; dialog->profile = job->profile;
                GtkStringList *names = gtk_string_list_new(NULL);
                for (size_t i = 0U; i < job->summary.file_count; ++i) {
                    UmiApplicationScaffoldFile file;
                    if (umi_application_scaffold_file(form->plan, i, &file) == UMI_STATUS_OK)
                        gtk_string_list_append(names, file.relative_path);
                }
                form->changing = 1;
                gtk_drop_down_set_model(GTK_DROP_DOWN(form->files), G_LIST_MODEL(names));
                g_object_unref(names);
                gtk_drop_down_set_selected(GTK_DROP_DOWN(form->files), 0U);
                form->changing = 0;
                if (DeveloperVisible(dialog) && form->plan != NULL && !form->invalidated) {
                    UmiApplicationScaffoldFile first;
                    if (umi_application_scaffold_file(form->plan, 0U, &first) == UMI_STATUS_OK)
                        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(form->text)),
                            first.text, (int)first.length);
                }
                if (DeveloperVisible(dialog) && !form->invalidated) {
                    char message[UMI_PATH_CAPACITY + 256U];
                    (void)g_snprintf(message, sizeof message,
                        "Review %zu files (%zu bytes) for %s. Create Source writes this reviewed snapshot.",
                        job->summary.file_count, job->summary.byte_count, job->summary.root);
                    ShowStatus(dialog, UMI_STATUS_OK, message);
                }
            } else ShowStatus(dialog, job->status, NULL);
        }
    }
    form->busy = 0;
    form->applying = 0;
    if (DeveloperAlive(dialog)) DeveloperApplicationControls(dialog);
    DeveloperRelease(dialog);
}

/* Transfer the acquired dialog reference to the completion. No cancellation is
 * advertised for writes: stopping halfway cannot restore their previous state. */
static void DeveloperApplicationStart(UmiGtk4DeveloperDialog *dialog, DeveloperApplicationJob *job)
{
    DeveloperApplicationForm *form = dialog->application;
    form->busy = 1;
    form->applying = job->apply;
    GTask *task = g_task_new(NULL, NULL, DeveloperApplicationCompleted, dialog);
    g_task_set_task_data(task, job, DeveloperApplicationJobFree);
    DeveloperApplicationControls(dialog);
    ShowStatus(dialog, UMI_STATUS_OK, job->apply
        ? "Creating reviewed source. No compiler, Git command or application will run."
        : "Reading templates and preparing a source preview...");
    /* Widget notifications may retire the host before the job starts. Retire
     * the unstarted task too; after the worker starts, writes finish normally. */
    if (!DeveloperVisible(dialog) || (!job->apply && form->invalidated)) {
        form->busy = 0; form->applying = 0;
        g_object_unref(task);
        DeveloperRelease(dialog);
        return;
    }
    g_task_run_in_thread(task, DeveloperApplicationWorker);
    g_object_unref(task);
}

static void DeveloperApplicationPreview(GtkButton *button, gpointer context)
{
    (void)button;
    UmiGtk4DeveloperDialog *dialog = DeveloperAcquire(context);
    if (dialog == NULL) return;
    DeveloperApplicationForm *form = dialog->application;
    if (form->busy || dialog->projectChoosing || dialog->generated || form->revisionExhausted) {
        DeveloperRelease(dialog); return;
    }
    DeveloperApplicationJob *job = calloc(1U, sizeof *job);
    if (job == NULL) {
        ShowStatus(dialog, UMI_STATUS_OUT_OF_MEMORY, NULL);
        DeveloperRelease(dialog); return;
    }
    UmiStatus status = UMI_STATUS_OK;
    for (size_t i = 0U; i < 5U && status == UMI_STATUS_OK; ++i)
        status = umi_text_copy(job->fields[i], sizeof job->fields[i],
            gtk_editable_get_text(GTK_EDITABLE(dialog->fields[i])));
    if (status != UMI_STATUS_OK) {
        free(job); ShowStatus(dialog, status, NULL); DeveloperRelease(dialog); return;
    }
    job->gtk = gtk_check_button_get_active(GTK_CHECK_BUTTON(form->gtk));
    DeveloperApplicationInvalidate(dialog);
    form->invalidated = 0;
    DeveloperApplicationStart(dialog, job);
}

static void DeveloperApplicationAccept(GtkButton *button, gpointer context)
{
    (void)button;
    UmiGtk4DeveloperDialog *dialog = DeveloperAcquire(context);
    if (dialog == NULL) return;
    DeveloperApplicationForm *form = dialog->application;
    if (form->busy || dialog->projectChoosing || dialog->accepting) {
        DeveloperRelease(dialog); return;
    }
    if (dialog->generated) {
        dialog->accepting = 1;
        UmiStatus status = dialog->onCreated(&dialog->model, &dialog->profile, dialog->context);
        dialog->accepting = 0;
        if (DeveloperVisible(dialog)) {
            if (status == UMI_STATUS_OK) gtk_widget_set_visible(GTK_WIDGET(dialog->window), FALSE);
            else ShowStatus(dialog, status,
                "The source is safe in its chosen folder. Finish active work and retry Open Project, or use Open Folder.");
        }
        DeveloperRelease(dialog); return;
    }
    if (form->plan == NULL || form->invalidated || form->revisionExhausted) { DeveloperRelease(dialog); return; }
    DeveloperApplicationJob *job = calloc(1U, sizeof *job);
    if (job == NULL) {
        ShowStatus(dialog, UMI_STATUS_OUT_OF_MEMORY, NULL);
        DeveloperRelease(dialog); return;
    }
    job->apply = 1;
    job->plan = form->plan; form->plan = NULL;
    DeveloperApplicationStart(dialog, job);
}

/* Hiding a preview ends its authority even if a retained window is presented
 * again. A write already accepted may finish, but never calls the host itself. */
static void DeveloperApplicationVisibility(GObject *object, GParamSpec *property, gpointer context)
{
    (void)object; (void)property;
    UmiGtk4DeveloperDialog *dialog = context;
    ++dialog->references;
    if (DeveloperVisible(dialog)) DeveloperApplicationControls(dialog);
    else {
        DeveloperApplicationInvalidate(dialog);
        if (dialog->projectChooserCancel != NULL) g_cancellable_cancel(dialog->projectChooserCancel);
    }
    DeveloperRelease(dialog);
}

typedef struct DeveloperApplicationChoice {
    UmiGtk4DeveloperDialog *dialog;
    char previous[UMI_PATH_CAPACITY];
    uint64_t revision;
} DeveloperApplicationChoice;

static void DeveloperApplicationFolderChosen(GObject *source, GAsyncResult *result, gpointer context)
{
    DeveloperApplicationChoice *choice = context;
    UmiGtk4DeveloperDialog *dialog = choice->dialog;
    GError *error = NULL;
    GFile *file = gtk_file_dialog_select_folder_finish(GTK_FILE_DIALOG(source), result, &error);
    char *path = file == NULL ? NULL : g_file_get_path(file);
    if (DeveloperVisible(dialog) && !dialog->generated &&
        !dialog->application->revisionExhausted &&
        choice->revision == dialog->application->editRevision &&
        strcmp(choice->previous, gtk_editable_get_text(GTK_EDITABLE(dialog->fields[4]))) == 0) {
        if (path != NULL && g_utf8_validate(path, -1, NULL))
            gtk_editable_set_text(GTK_EDITABLE(dialog->fields[4]), path);
        else if (file != NULL) ShowStatus(dialog, UMI_STATUS_INVALID_ARGUMENT,
            "Choose a local filesystem folder, or enter its absolute path.");
    }
    g_free(path); g_clear_object(&file); g_clear_error(&error);
    g_clear_object(&dialog->projectChooserCancel);
    dialog->projectChoosing = 0;
    if (DeveloperVisible(dialog)) DeveloperApplicationControls(dialog);
    free(choice);
    DeveloperRelease(dialog);
}

static void DeveloperApplicationChooseFolder(GtkButton *button, gpointer context)
{
    (void)button;
    UmiGtk4DeveloperDialog *dialog = DeveloperAcquire(context);
    if (dialog == NULL) return;
    DeveloperApplicationForm *form = dialog->application;
    if (form->busy || dialog->projectChoosing || dialog->generated || form->revisionExhausted) {
        DeveloperRelease(dialog); return;
    }
    DeveloperApplicationChoice *choice = calloc(1U, sizeof *choice);
    if (choice == NULL) { DeveloperRelease(dialog); return; }
    choice->dialog = dialog;
    choice->revision = form->editRevision;
    UmiStatus status = umi_text_copy(choice->previous, sizeof choice->previous,
        gtk_editable_get_text(GTK_EDITABLE(dialog->fields[4])));
    if (status != UMI_STATUS_OK) {
        free(choice); ShowStatus(dialog, status, NULL); DeveloperRelease(dialog); return;
    }
    dialog->projectChoosing = 1;
    dialog->projectChooserCancel = g_cancellable_new();
    GtkFileDialog *picker = gtk_file_dialog_new();
    gtk_file_dialog_set_title(picker, "Choose where to create the application folder");
    gtk_file_dialog_set_modal(picker, TRUE);
    if (umi_path_is_absolute(choice->previous)) {
        GFile *folder = g_file_new_for_path(choice->previous);
        gtk_file_dialog_set_initial_folder(picker, folder);
        g_object_unref(folder);
    }
    DeveloperApplicationControls(dialog);
    gtk_file_dialog_select_folder(picker, dialog->window, dialog->projectChooserCancel,
        DeveloperApplicationFolderChosen, choice);
    g_object_unref(picker);
}

/* Application creation now supports hidden preparation and checked presentation. The earlier direct presentation is retained for review. */
#if 0
UmiStatus UmiGtk4ApplicationModuleDialogCreate(GtkWindow *parent,
    UmiGtk4ProjectCreated onCreated, void *context, UmiGtk4DeveloperDialog **outDialog)
{
    if (outDialog == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outDialog = NULL;
    if (parent == NULL || onCreated == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiGtk4DeveloperDialog *dialog = NULL;
    UmiStatus status = CreateForm(parent, "New Umicom Application",
        "Create a C23 application with a manifest, tests and optional GTK frontend. "
        "Choose its parent folder, preview every source file, then create it. "
        "Repository registration and publication are separate reviewed actions.",
        G_CALLBACK(DeveloperApplicationAccept), "Create Source", &dialog);
    if (status != UMI_STATUS_OK) return status;
    DeveloperApplicationForm *form = calloc(1U, sizeof *form);
    if (form == NULL) { UmiGtk4DeveloperDialogDestroy(dialog); return UMI_STATUS_OUT_OF_MEMORY; }
    dialog->application = form; dialog->onCreated = onCreated; dialog->context = context;
    dialog->fields[0] = AddEntry(dialog, "Application name", "Umicom Notes", "developer.application.name");
    dialog->fields[1] = AddEntry(dialog, "Application identifier", "org.umicom.notes", "developer.application.id");
    dialog->fields[2] = AddEntry(dialog, "New folder and module slug", "notes", "developer.application.slug");
    dialog->fields[3] = AddEntry(dialog, "Template folder (blank uses Framework resources)", "", "developer.application.templates");
    dialog->fields[4] = AddEntry(dialog, "Parent folder (absolute path)", "", "developer.application.parent");
    form->choose = gtk_button_new_with_label("Choose Parent Folder...");
    form->gtk = gtk_check_button_new_with_label("Include a GTK desktop frontend");
    gtk_check_button_set_active(GTK_CHECK_BUTTON(form->gtk), TRUE);
    form->preview = gtk_button_new_with_label("Preview Source");
    form->files = gtk_drop_down_new_from_strings((const char *[]) {NULL});
    form->text = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(form->text), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(form->text), TRUE);
    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll), 240);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), form->text);
    gtk_box_append(GTK_BOX(dialog->body), form->choose);
    gtk_box_append(GTK_BOX(dialog->body), form->gtk);
    gtk_box_append(GTK_BOX(dialog->body), form->preview);
    gtk_box_append(GTK_BOX(dialog->body), form->files);
    gtk_box_append(GTK_BOX(dialog->body), scroll);
    (void)umi_gtk4_automation_tag_widget(form->choose, "developer.application.choose-parent");
    (void)umi_gtk4_automation_tag_widget(form->gtk, "developer.application.gtk");
    (void)umi_gtk4_automation_tag_widget(form->preview, "developer.application.preview");
    (void)umi_gtk4_automation_tag_widget(form->files, "developer.application.files");
    (void)umi_gtk4_automation_tag_widget(form->text, "developer.application.source");
    (void)umi_gtk4_automation_tag_widget(dialog->message, "developer.application.status");
    for (size_t i = 0U; i < 5U; ++i)
        DeveloperConnect(dialog, G_OBJECT(dialog->fields[i]), "changed", G_CALLBACK(DeveloperApplicationEdited));
    DeveloperConnect(dialog, G_OBJECT(form->gtk), "toggled", G_CALLBACK(DeveloperApplicationToggle));
    DeveloperConnect(dialog, G_OBJECT(form->choose), "clicked", G_CALLBACK(DeveloperApplicationChooseFolder));
    DeveloperConnect(dialog, G_OBJECT(form->preview), "clicked", G_CALLBACK(DeveloperApplicationPreview));
    DeveloperConnect(dialog, G_OBJECT(form->files), "notify::selected", G_CALLBACK(DeveloperApplicationFileSelected));
    DeveloperConnect(dialog, G_OBJECT(dialog->window), "notify::visible", G_CALLBACK(DeveloperApplicationVisibility));
    DeveloperApplicationControls(dialog);
    *outDialog = dialog;
    gtk_window_present(dialog->window);
    return UMI_STATUS_OK;
}

#endif

UmiStatus UmiGtk4ApplicationModuleDialogPrepare(GtkWindow *parent,
    UmiGtk4ProjectCreated onCreated, void *context, UmiGtk4DeveloperDialog **outDialog)
{
    if (outDialog == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outDialog = NULL;
    if (parent == NULL || onCreated == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiGtk4DeveloperDialog *dialog = NULL;
    UmiStatus status = CreateForm(parent, "New Umicom Application",
        "Create a C23 application with a manifest, tests and optional GTK frontend. "
        "Choose its parent folder, preview every source file, then create it. "
        "Repository registration and publication are separate reviewed actions.",
        G_CALLBACK(DeveloperApplicationAccept), "Create Source", &dialog);
    if (status != UMI_STATUS_OK) return status;
    DeveloperApplicationForm *form = calloc(1U, sizeof *form);
    if (form == NULL) { UmiGtk4DeveloperDialogDestroy(dialog); return UMI_STATUS_OUT_OF_MEMORY; }
    dialog->application = form; dialog->onCreated = onCreated; dialog->context = context;
    dialog->fields[0] = AddEntry(dialog, "Application name", "Umicom Notes", "developer.application.name");
    dialog->fields[1] = AddEntry(dialog, "Application identifier", "org.umicom.notes", "developer.application.id");
    dialog->fields[2] = AddEntry(dialog, "New folder and module slug", "notes", "developer.application.slug");
    dialog->fields[3] = AddEntry(dialog, "Template folder (blank uses Framework resources)", "", "developer.application.templates");
    dialog->fields[4] = AddEntry(dialog, "Parent folder (absolute path)", "", "developer.application.parent");
    form->choose = gtk_button_new_with_label("Choose Parent Folder...");
    form->gtk = gtk_check_button_new_with_label("Include a GTK desktop frontend");
    gtk_check_button_set_active(GTK_CHECK_BUTTON(form->gtk), TRUE);
    form->preview = gtk_button_new_with_label("Preview Source");
    form->files = gtk_drop_down_new_from_strings((const char *[]) {NULL});
    form->text = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(form->text), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(form->text), TRUE);
    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll), 240);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), form->text);
    gtk_box_append(GTK_BOX(dialog->body), form->choose);
    gtk_box_append(GTK_BOX(dialog->body), form->gtk);
    gtk_box_append(GTK_BOX(dialog->body), form->preview);
    gtk_box_append(GTK_BOX(dialog->body), form->files);
    gtk_box_append(GTK_BOX(dialog->body), scroll);
    (void)umi_gtk4_automation_tag_widget(form->choose, "developer.application.choose-parent");
    (void)umi_gtk4_automation_tag_widget(form->gtk, "developer.application.gtk");
    (void)umi_gtk4_automation_tag_widget(form->preview, "developer.application.preview");
    (void)umi_gtk4_automation_tag_widget(form->files, "developer.application.files");
    (void)umi_gtk4_automation_tag_widget(form->text, "developer.application.source");
    (void)umi_gtk4_automation_tag_widget(dialog->message, "developer.application.status");
    for (size_t i = 0U; i < 5U; ++i)
        DeveloperConnect(dialog, G_OBJECT(dialog->fields[i]), "changed", G_CALLBACK(DeveloperApplicationEdited));
    DeveloperConnect(dialog, G_OBJECT(form->gtk), "toggled", G_CALLBACK(DeveloperApplicationToggle));
    DeveloperConnect(dialog, G_OBJECT(form->choose), "clicked", G_CALLBACK(DeveloperApplicationChooseFolder));
    DeveloperConnect(dialog, G_OBJECT(form->preview), "clicked", G_CALLBACK(DeveloperApplicationPreview));
    DeveloperConnect(dialog, G_OBJECT(form->files), "notify::selected", G_CALLBACK(DeveloperApplicationFileSelected));
    DeveloperConnect(dialog, G_OBJECT(dialog->window), "notify::visible", G_CALLBACK(DeveloperApplicationVisibility));
    DeveloperApplicationControls(dialog);
    if (!DeveloperAlive(dialog)) { UmiGtk4DeveloperDialogDestroy(dialog); return UMI_STATUS_CANCELLED; }
    *outDialog = dialog;
    return UMI_STATUS_OK;
}


/* Keep the established immediate-opening API for callers with stable output
 * storage; disposable hosts adopt a prepared form before presenting it. */
UmiStatus UmiGtk4ApplicationModuleDialogCreate(GtkWindow *parent,
    UmiGtk4ProjectCreated onCreated, void *context, UmiGtk4DeveloperDialog **outDialog)
{
    UmiStatus status = UmiGtk4ApplicationModuleDialogPrepare(parent, onCreated, context, outDialog);
    if (status != UMI_STATUS_OK) return status;
    return UmiGtk4DeveloperDialogPresent(*outDialog);
}