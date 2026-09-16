/*-----------------------------------------------------------------------------
 * Umicom Framework — application development lessons
 * File: examples/learning/projects/notes_model.c
 * PURPOSE: Reuse Framework documents and commands; keep GTK out of the model.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "notes_model.h"
#include "umicom/base/text.h"
#include "umicom/platform/filesystem.h"
#include <stdlib.h>
#include <string.h>

struct UmiLearningNotes {
    UmiCommandRegistry *commands;
    UmiUiWorkbench *workbench;
    UmiDocumentStore *store;
    UmiDocumentCoordinator *documents;
};

/* A save uses the coordinator's atomic writer and external-change checks. */
static UmiStatus SaveNote(void *context, const char *argument, char *message, size_t capacity)
{
    UmiLearningNotes *notes = context;
    UmiDocumentWorkingCopySnapshot state;
    char current[UMI_PATH_CAPACITY], destination[UMI_PATH_CAPACITY];
    UmiStatus status;
    (void)message; (void)capacity;
    if (argument == NULL || argument[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_fs_current_directory(current, sizeof(current));
    if (status == UMI_STATUS_OK)
        status = umi_path_absolute(argument, current, destination, sizeof(destination));
    if (status != UMI_STATUS_OK) return status;
    status = umi_document_coordinator_active_snapshot(notes->documents, &state);
    if (status != UMI_STATUS_OK) return status;
    if (state.has_path && umi_path_equal(state.path, destination))
        return umi_document_coordinator_save_active(notes->documents);
    /* Do not overwrite an unrelated existing note in a beginner exercise. */
    if (umi_fs_exists(destination)) return UMI_STATUS_ALREADY_EXISTS;
    return umi_document_coordinator_save_active_as(notes->documents, destination);
}

static UmiStatus NewNote(void *context, const char *argument, char *message, size_t capacity)
{
    UmiLearningNotes *notes = context;
    UmiDocumentWorkingCopySnapshot state;
    UmiStatus status = umi_document_coordinator_active_snapshot(notes->documents, &state);
    (void)argument; (void)message; (void)capacity;
    if (status == UMI_STATUS_OK) {
        if (state.dirty && (state.has_path || state.text_length != 0U)) return UMI_STATUS_INVALID_STATE;
        status = umi_document_coordinator_close_active(notes->documents, 1);
        if (status != UMI_STATUS_OK) return status;
    } else if (status != UMI_STATUS_NOT_FOUND) return status;
    return umi_document_coordinator_new(notes->documents, "UmicomNote.txt", NULL, 0U);
}

static UmiStatus UndoNote(void *context, const char *argument, char *message, size_t capacity)
{
    UmiLearningNotes *notes = context;
    (void)argument; (void)message; (void)capacity;
    return umi_document_coordinator_undo(notes->documents);
}

UmiStatus UmiLearningNotesCreate(UmiLearningNotes **outNotes)
{
    UmiLearningNotes *notes;
    UmiStatus status;
    if (outNotes == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outNotes = NULL;
    notes = calloc(1U, sizeof(*notes));
    if (notes == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_command_registry_create(&notes->commands);
    if (status == UMI_STATUS_OK)
        status = umi_ui_workbench_create("learning.notes", notes->commands, &notes->workbench);
    if (status == UMI_STATUS_OK) status = umi_document_store_create(&notes->store);
    if (status == UMI_STATUS_OK)
        status = umi_document_coordinator_create(notes->store, notes->workbench, NULL, &notes->documents);
    UmiCommandDescriptor commands[] = {
        {sizeof(UmiCommandDescriptor), "notes.new", "New Note", "File", "Create an empty note without discarding changes.", "", UMI_COMMAND_MUTATES_STATE, NewNote, NULL, notes},
        {sizeof(UmiCommandDescriptor), "notes.save", "Save Note", "File", "Save through the Framework document coordinator.", "", UMI_COMMAND_MUTATES_STATE, SaveNote, NULL, notes},
        {sizeof(UmiCommandDescriptor), "notes.undo", "Undo", "Edit", "Restore the previous document text.", "", UMI_COMMAND_MUTATES_STATE, UndoNote, NULL, notes}
    };
    for (size_t index = 0U; status == UMI_STATUS_OK && index < sizeof(commands)/sizeof(commands[0]); ++index)
        status = umi_command_registry_register(notes->commands, &commands[index]);
    if (status == UMI_STATUS_OK) status = NewNote(notes, NULL, NULL, 0U);
    if (status != UMI_STATUS_OK) { UmiLearningNotesDestroy(notes); return status; }
    *outNotes = notes;
    return UMI_STATUS_OK;
}

void UmiLearningNotesDestroy(UmiLearningNotes *notes)
{
    if (notes == NULL) return;
    umi_document_coordinator_destroy(notes->documents);
    umi_document_store_destroy(notes->store);
    umi_ui_workbench_destroy(notes->workbench);
    umi_command_registry_destroy(notes->commands);
    free(notes);
}

UmiStatus UmiLearningNotesSetText(UmiLearningNotes *notes, const char *text)
{
    UmiDocumentWorkingCopySnapshot state;
    UmiUiDocumentViewSnapshot *view;
    UmiStatus status;
    if (notes == NULL || text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (strlen(text) >= UMI_UI_DOCUMENT_CONTENT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = UmiLearningNotesSnapshot(notes, &state);
    if (status != UMI_STATUS_OK) return status;
    view = calloc(1U, sizeof(*view));
    if (view == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_ui_document_view_model_find(umi_ui_workbench_documents(notes->workbench), state.view_id, view);
    if (status == UMI_STATUS_OK) status = umi_text_copy(view->source_text, sizeof(view->source_text), text);
    if (status == UMI_STATUS_OK) {
        view->dirty = 1;
        status = umi_ui_document_view_model_upsert(umi_ui_workbench_documents(notes->workbench), view);
    }
    if (status == UMI_STATUS_OK) status = umi_document_coordinator_sync_active(notes->documents);
    free(view);
    return status;
}

UmiStatus UmiLearningNotesCopyText(UmiLearningNotes *notes, char **outText)
{
    UmiDocumentWorkingCopySnapshot state;
    if (notes == NULL || outText == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outText = NULL;
    UmiStatus status = UmiLearningNotesSnapshot(notes, &state);
    return status == UMI_STATUS_OK ? umi_document_store_copy_text(notes->store, state.document_id, outText, NULL) : status;
}

UmiStatus UmiLearningNotesSnapshot(UmiLearningNotes *notes, UmiDocumentWorkingCopySnapshot *outState)
{
    return notes != NULL ? umi_document_coordinator_active_snapshot(notes->documents, outState) : UMI_STATUS_INVALID_ARGUMENT;
}

UmiStatus UmiLearningNotesExecute(UmiLearningNotes *notes, const char *command, const char *argument)
{
    return notes != NULL ? umi_command_registry_execute(notes->commands, command, argument, NULL, 0U) : UMI_STATUS_INVALID_ARGUMENT;
}
