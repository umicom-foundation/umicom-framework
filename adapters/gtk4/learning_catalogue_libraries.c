/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/learning_catalogue_libraries.c
 *
 * PURPOSE:
 *   Capture explicit library choices and persist them with independent asynchronous ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "learning_catalogue_private.h"
#include "umicom/ui/gtk4/path_picker.h"
#include "umicom/platform/output_file.h"
#include <inttypes.h>
#include <string.h>

static UmiStatus LibraryReady(LearningCatalogue *owner)
{
    if (owner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (owner->state.retired) return UMI_STATUS_INVALID_STATE;
    return owner->state.busy ? UMI_STATUS_BUSY : UMI_STATUS_OK;
}
static void LibraryRows(LearningCatalogue *owner)
{
    GtkStringList *rows = gtk_string_list_new(NULL);
    owner->state.library_choice_count = UmiLearningLibraryChoicesCount(owner->library_choices);
    for (size_t i = 0U; i < owner->state.library_choice_count; ++i) {
        UmiLearningLibraryChoice entry;
        if (UmiLearningLibraryChoicesAt(owner->library_choices, i, &entry) != UMI_STATUS_OK) break;
        char *text = g_strdup_printf("%s [%s] revision %" PRIu64 " — %s",
            entry.pack_id, entry.locale, entry.revision, entry.root);
        gtk_string_list_append(rows, text);
        g_free(text);
    }
    /* State is committed before model notifications. Mutating callers hold
     * busy, so a selection callback cannot change the list during publication. */
    gtk_drop_down_set_model(GTK_DROP_DOWN(owner->library_rows), G_LIST_MODEL(rows));
    if (!owner->state.retired)
        gtk_drop_down_set_selected(GTK_DROP_DOWN(owner->library_rows),
            owner->state.library_choice_count != 0U ? 0U : GTK_INVALID_LIST_POSITION);
    g_object_unref(rows);
}
UmiStatus UmiLearningCatalogueGtk4RememberLibrary(GtkWidget *widget)
{
    LearningCatalogue *owner = LearningCatalogueOwner(widget);
    UmiStatus status = LibraryReady(owner);
    if (status != UMI_STATUS_OK) return status;
    if (owner->reader == NULL || !owner->state.reader.has_document) return UMI_STATUS_INVALID_STATE;
    UmiLearningPack *pack = NULL;
    status = UmiLearningReaderCopyPack(owner->reader, &pack);
    if (status != UMI_STATUS_OK) return status;
    g_object_ref(widget);
    owner->state.busy = 1;
    status = UmiLearningLibraryChoicesRemember(owner->library_choices, pack, "learning-pack.json");
    UmiLearningPackDestroy(pack);
    if (status == UMI_STATUS_OK) LibraryRows(owner);
    if (!owner->state.retired) LearningCatalogueMessage(owner, status,
        "Library choice captured from the displayed pack. Save the list to keep it after closing.");
    owner->state.busy = 0;
    g_object_unref(widget);
    return status;
}
UmiStatus UmiLearningCatalogueGtk4ForgetLibrary(GtkWidget *widget, size_t index)
{
    LearningCatalogue *owner = LearningCatalogueOwner(widget);
    UmiStatus status = LibraryReady(owner);
    if (status != UMI_STATUS_OK) return status;
    UmiLearningLibraryChoice entry;
    status = UmiLearningLibraryChoicesAt(owner->library_choices, index, &entry);
    if (status != UMI_STATUS_OK) return status;
    g_object_ref(widget);
    owner->state.busy = 1;
    status = UmiLearningLibraryChoicesForget(owner->library_choices, entry.pack_id);
    if (status == UMI_STATUS_OK) LibraryRows(owner);
    if (!owner->state.retired) LearningCatalogueMessage(owner, status,
        "Forgot this folder choice. Loaded packs, reading history and files are unchanged.");
    owner->state.busy = 0;
    g_object_unref(widget);
    return status;
}
UmiStatus UmiLearningCatalogueGtk4LibraryChoiceAt(GtkWidget *widget, size_t index,
    UmiLearningLibraryChoice *out)
{
    LearningCatalogue *owner = LearningCatalogueOwner(widget);
    if (owner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return UmiLearningLibraryChoicesAt(owner->library_choices, index, out);
}
static UmiStatus LibraryDispatch(GtkWidget *widget, const char *filename, size_t index,
    LearningCatalogueAction action)
{
    LearningCatalogue *owner = LearningCatalogueOwner(widget);
    UmiStatus status = LibraryReady(owner);
    if (status != UMI_STATUS_OK) return status;
    LearningCatalogueJob *job = g_try_new0(LearningCatalogueJob, 1U);
    if (job == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    g_weak_ref_init(&job->root, widget);
    job->action = action; job->host = owner->host;
    UmiLearningCatalogueLibraryReceipt *receipt = &job->library_receipt;
    if (action == LEARNING_CATALOGUE_LIBRARY_REOPEN) {
        receipt->action = UMI_LEARNING_CATALOGUE_LIBRARY_REOPEN;
        receipt->choice_index = index;
        status = UmiLearningLibraryChoicesAt(owner->library_choices, index, &receipt->choice);
    } else {
        receipt->action = action == LEARNING_CATALOGUE_LIBRARIES_LOAD ?
            UMI_LEARNING_CATALOGUE_LIBRARIES_LOAD : UMI_LEARNING_CATALOGUE_LIBRARIES_SAVE_NEW;
        /* Capture an ordinary full filename; rooted storage rejects ambiguous
         * components and reparse points when the worker opens it. */
        char copied[UMI_PATH_CAPACITY];
        status = filename == NULL ? UMI_STATUS_INVALID_ARGUMENT :
            umi_path_copy(copied, sizeof(copied), filename);
        if (status == UMI_STATUS_OK && (!umi_path_is_absolute(copied) || !g_utf8_validate(copied, -1, NULL)))
            status = UMI_STATUS_INVALID_ARGUMENT;
        /* Validate the original spelling before path helpers normalise it,
         * so dot segments cannot acquire a different meaning during capture. */
        if (status == UMI_STATUS_OK) status = UmiOutputFileValidatePath(copied);
        if (status == UMI_STATUS_OK) status = umi_path_parent(copied, receipt->settings_root,
            sizeof(receipt->settings_root));
        if (status == UMI_STATUS_OK) status = umi_path_basename(copied, receipt->settings_path,
            sizeof(receipt->settings_path));
        if (status == UMI_STATUS_OK) status = UmiRootedFileValidatePath(receipt->settings_root, receipt->settings_path);
    }
    if (status == UMI_STATUS_OK && action != LEARNING_CATALOGUE_LIBRARIES_LOAD)
        status = UmiLearningLibraryChoicesClone(owner->library_choices, &job->library_choices);
    if (status != UMI_STATUS_OK) { LearningCatalogueJobFree(job); return status; }
    receipt->choice_count = UmiLearningLibraryChoicesCount(owner->library_choices);
    g_object_ref(widget);
    status = LearningCatalogueDispatch(owner, job);
    g_object_unref(widget);
    return status;
}
UmiStatus UmiLearningCatalogueGtk4LoadLibraries(GtkWidget *widget, const char *file)
{ return LibraryDispatch(widget, file, 0U, LEARNING_CATALOGUE_LIBRARIES_LOAD); }
UmiStatus UmiLearningCatalogueGtk4SaveLibrariesNew(GtkWidget *widget, const char *file)
{ return LibraryDispatch(widget, file, 0U, LEARNING_CATALOGUE_LIBRARIES_SAVE); }
UmiStatus UmiLearningCatalogueGtk4ReopenLibrary(GtkWidget *widget, size_t index)
{ return LibraryDispatch(widget, NULL, index, LEARNING_CATALOGUE_LIBRARY_REOPEN); }

void LearningCatalogueLibraryWorker(LearningCatalogueJob *job)
{
    UmiLearningCatalogueLibraryReceipt *receipt = &job->library_receipt;
    if (job->action == LEARNING_CATALOGUE_LIBRARIES_LOAD) {
        job->status = UmiLearningLibraryChoicesLoad(receipt->settings_root, receipt->settings_path,
            &job->library_choices);
        if (job->status == UMI_STATUS_OK)
            receipt->choice_count = UmiLearningLibraryChoicesCount(job->library_choices);
    } else if (job->action == LEARNING_CATALOGUE_LIBRARIES_SAVE) {
        job->status = UmiLearningLibraryChoicesSaveNew(job->library_choices,
            receipt->settings_root, receipt->settings_path, &receipt->creation);
    } else {
        job->status = UmiLearningLibraryChoicesReopen(job->library_choices,
            receipt->choice_index, job->host, &job->pack);
    }
    receipt->status = job->status;
}
void LearningCatalogueLibraryComplete(LearningCatalogue *owner, LearningCatalogueJob *job)
{
    if (job->status == UMI_STATUS_OK && job->action == LEARNING_CATALOGUE_LIBRARIES_LOAD) {
        UmiLearningLibraryChoicesDestroy(owner->library_choices);
        owner->library_choices = job->library_choices;
        job->library_choices = NULL;
        LibraryRows(owner);
    } else if (job->status == UMI_STATUS_OK && job->action == LEARNING_CATALOGUE_LIBRARY_REOPEN) {
        UmiLearningShelf *candidate = NULL;
        job->status = UmiLearningShelfClone(owner->shelf, &candidate);
        if (job->status == UMI_STATUS_OK) job->status = UmiLearningShelfAdopt(candidate, &job->pack);
        if (job->status == UMI_STATUS_OK) job->status = LearningCataloguePublishShelf(owner,
            &candidate, gtk_editable_get_text(GTK_EDITABLE(owner->query)));
        UmiLearningShelfDestroy(candidate);
    }
    job->library_receipt.status = job->status;
    owner->library_receipt = job->library_receipt;
    owner->state.has_library_result = 1;
    /* A model notification can retire the panel. Retain the plain receipt but
     * publish no further widget changes after retirement. */
    if (owner->state.retired) return;
    const UmiLearningCatalogueLibraryReceipt *receipt = &owner->library_receipt;
    const char *action = receipt->action == UMI_LEARNING_CATALOGUE_LIBRARIES_LOAD ? "Load folder choices" :
        receipt->action == UMI_LEARNING_CATALOGUE_LIBRARIES_SAVE_NEW ? "Save new folder-choices file" : "Reopen chosen library";
    char *message = g_strdup_printf("%s: %s\nChoices captured: %zu\n%s%s%s%s",
        action, umi_status_text(receipt->status), receipt->choice_count,
        receipt->action == UMI_LEARNING_CATALOGUE_LIBRARY_REOPEN ? "Pack: " : "Settings: ",
        receipt->action == UMI_LEARNING_CATALOGUE_LIBRARY_REOPEN ? receipt->choice.pack_id : receipt->settings_root,
        receipt->action == UMI_LEARNING_CATALOGUE_LIBRARY_REOPEN ? "" : "/",
        receipt->action == UMI_LEARNING_CATALOGUE_LIBRARY_REOPEN ? "" : receipt->settings_path);
    gtk_label_set_text(GTK_LABEL(owner->library_status), message);
    g_free(message);
    if (!owner->state.retired) LearningCatalogueMessage(owner, job->status,
        receipt->creation.created && !receipt->creation.complete ?
            "A new settings file is incomplete. Inspect it before choosing a different filename." :
        job->action == LEARNING_CATALOGUE_LIBRARIES_LOAD ?
            "Loading choices does not open their folders. Select a library and explicitly reopen it." :
        job->action == LEARNING_CATALOGUE_LIBRARIES_SAVE ?
            "Existing settings files are never overwritten. Keep personal folder choices outside repositories." :
            "A reopened library is added only after admission. Existing reading history is kept.");
}
UmiStatus UmiLearningCatalogueGtk4CopyLibraryReceipt(GtkWidget *widget,
    UmiLearningCatalogueLibraryReceipt *out)
{
    LearningCatalogue *owner = LearningCatalogueOwner(widget);
    if (owner == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!owner->state.has_library_result) return UMI_STATUS_NOT_FOUND;
    *out = owner->library_receipt;
    return UMI_STATUS_OK;
}

static void LibraryAction(GtkButton *button, gpointer data)
{
    GtkWidget *widget = data;
    g_object_ref(widget);
    LearningCatalogue *owner = LearningCatalogueOwner(widget);
    if (LibraryReady(owner) == UMI_STATUS_OK) {
        unsigned action = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(button), "learning-library-action"));
        guint index = gtk_drop_down_get_selected(GTK_DROP_DOWN(owner->library_rows));
        const char *file = gtk_editable_get_text(GTK_EDITABLE(owner->library_path));
        UmiStatus status = action == 1U ? UmiLearningCatalogueGtk4RememberLibrary(widget) :
            action == 2U ? UmiLearningCatalogueGtk4ForgetLibrary(widget, (size_t)index) :
            action == 3U ? UmiLearningCatalogueGtk4LoadLibraries(widget, file) :
            action == 4U ? UmiLearningCatalogueGtk4SaveLibrariesNew(widget, file) :
                UmiLearningCatalogueGtk4ReopenLibrary(widget, (size_t)index);
        if (status != UMI_STATUS_OK && !owner->state.retired)
            LearningCatalogueMessage(owner, status, "Check the selected library and private settings filename.");
    }
    g_object_unref(widget);
}
static void LibraryTag(GtkWidget *widget, const char *name)
{
    char *id = g_strdup_printf("learning.catalogue.libraries-%s", name);
    g_object_set_data_full(G_OBJECT(widget), "umicom-automation-id", id, g_free);
}
static GtkWidget *LibraryLabel(GtkWidget *box, const char *text)
{
    GtkWidget *label = gtk_label_new(text);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_box_append(GTK_BOX(box), label);
    return label;
}
static void LibraryButton(LearningCatalogue *owner, GtkWidget *box,
    const char *label, const char *id, unsigned action)
{
    GtkWidget *button = gtk_button_new_with_label(label);
    LibraryTag(button, id);
    g_object_set_data(G_OBJECT(button), "learning-library-action", GUINT_TO_POINTER(action));
    g_signal_connect_object(button, "clicked", G_CALLBACK(LibraryAction), owner->root, 0);
    gtk_box_append(GTK_BOX(box), button);
}
void LearningCatalogueLibraryControls(LearningCatalogue *owner)
{
    GtkWidget *expander = gtk_expander_new("Remembered learning libraries");
    LibraryTag(expander, "section");
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    LibraryLabel(box, "Remember the displayed pack's folder, then save the list in a private file. "
        "Loading a list does not open folders. Reopening checks each chosen manifest again.");
    LibraryButton(owner, box, "Remember displayed library", "remember", 1U);
    owner->library_rows = gtk_drop_down_new(G_LIST_MODEL(gtk_string_list_new(NULL)), NULL);
    LibraryTag(owner->library_rows, "choices");
    gtk_box_append(GTK_BOX(box), owner->library_rows);
    LibraryButton(owner, box, "Reopen selected library", "reopen", 5U);
    LibraryButton(owner, box, "Forget selected folder choice", "forget", 2U);
    LibraryLabel(box, "Private folder-choices filename");
    owner->library_path = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(owner->library_path), (int)UMI_PATH_CAPACITY - 1);
    gtk_entry_set_placeholder_text(GTK_ENTRY(owner->library_path), "Absolute local settings filename");
    LibraryTag(owner->library_path, "path");
    gtk_box_append(GTK_BOX(box), owner->library_path);
    GtkWidget *load = UmiPathPickerGtk4Create(GTK_ENTRY(owner->library_path),
        UMI_PATH_PICKER_EXISTING_FILE, "Choose a saved library list", NULL);
    GtkWidget *save = UmiPathPickerGtk4Create(GTK_ENTRY(owner->library_path),
        UMI_PATH_PICKER_NEW_FILE, "Choose a NEW private library-list filename", NULL);
    if (load != NULL) gtk_box_append(GTK_BOX(box), load);
    if (save != NULL) gtk_box_append(GTK_BOX(box), save);
    LibraryButton(owner, box, "Load folder choices", "load", 3U);
    LibraryButton(owner, box, "Save choices to new file", "save", 4U);
    owner->library_status = LibraryLabel(box, "No settings operation has completed.");
    LibraryTag(owner->library_status, "result");
    gtk_expander_set_child(GTK_EXPANDER(expander), box);
    gtk_box_append(GTK_BOX(owner->controls), expander);
}
