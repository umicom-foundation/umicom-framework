/*-----------------------------------------------------------------------------
 * Umicom Framework — application development lessons
 * File: examples/learning/projects/test_notes_model.c
 * PURPOSE: Check the real Notes model without requiring a graphical display.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "notes_model.h"
#include "umicom/platform/filesystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(test) do { if (!(test)) { fprintf(stderr, "line %d: %s\n", __LINE__, #test); return EXIT_FAILURE; } } while (0)
int main(void)
{
    UmiLearningNotes *notes = NULL;
    char *text = NULL;
    UmiDocumentWorkingCopySnapshot state;
    CHECK(!umi_fs_exists("notes.txt"));
    CHECK(UmiLearningNotesCreate(&notes) == UMI_STATUS_OK);
    CHECK(UmiLearningNotesSetText(notes, "first draft\n") == UMI_STATUS_OK);
    CHECK(UmiLearningNotesSetText(notes, "second draft\n") == UMI_STATUS_OK);
    CHECK(UmiLearningNotesExecute(notes, "notes.new", NULL) == UMI_STATUS_INVALID_STATE);
    CHECK(UmiLearningNotesExecute(notes, "notes.undo", NULL) == UMI_STATUS_OK);
    CHECK(UmiLearningNotesCopyText(notes, &text) == UMI_STATUS_OK);
    CHECK(strcmp(text, "first draft\n") == 0); umi_document_store_free_text(text);
    CHECK(UmiLearningNotesExecute(notes, "notes.save", "notes.txt") == UMI_STATUS_OK);
    CHECK(UmiLearningNotesSnapshot(notes, &state) == UMI_STATUS_OK && !state.dirty);
    /* Deleting every character of a saved note is still an unsaved edit. */
    CHECK(UmiLearningNotesSetText(notes, "") == UMI_STATUS_OK);
    CHECK(UmiLearningNotesExecute(notes, "notes.new", NULL) == UMI_STATUS_INVALID_STATE);
    CHECK(UmiLearningNotesSetText(notes, "third draft\n") == UMI_STATUS_OK);
    CHECK(umi_fs_write_text("notes.txt", "external change\n") == UMI_STATUS_OK);
    CHECK(UmiLearningNotesExecute(notes, "notes.save", "notes.txt") == UMI_STATUS_INVALID_STATE);
    CHECK(umi_fs_read_text("notes.txt", &text, NULL) == UMI_STATUS_OK);
    CHECK(strcmp(text, "external change\n") == 0); umi_fs_free_text(text);
    CHECK(UmiLearningNotesExecute(notes, "missing.command", NULL) == UMI_STATUS_NOT_FOUND);
    UmiLearningNotesDestroy(notes); CHECK(remove("notes.txt") == 0);
    puts("Notes model checks passed."); return EXIT_SUCCESS;
}
