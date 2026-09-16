/*-----------------------------------------------------------------------------
 * Umicom Framework — application development lessons
 * File: examples/learning/projects/notes_session.c
 * PURPOSE: Run the Notes model from a terminal using the same commands as its window.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "notes_model.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    UmiLearningNotes *notes = NULL;
    char *text = NULL;
    UmiStatus status = UmiLearningNotesCreate(&notes);
    if (status == UMI_STATUS_OK) status = UmiLearningNotesSetText(notes, "Umicom Notes: review the build.\n");
    if (status == UMI_STATUS_OK) status = UmiLearningNotesSetText(notes, "Umicom Notes: review the build and tests.\n");
    if (status == UMI_STATUS_OK) status = UmiLearningNotesExecute(notes, "notes.undo", NULL);
    if (status == UMI_STATUS_OK) status = UmiLearningNotesCopyText(notes, &text);
    if (status == UMI_STATUS_OK) fputs(text, stdout);
    umi_document_store_free_text(text);
    if (status == UMI_STATUS_OK && argc == 2) status = UmiLearningNotesExecute(notes, "notes.save", argv[1]);
    if (status != UMI_STATUS_OK) fprintf(stderr, "Notes operation failed: %s\n", umi_status_text(status));
    UmiLearningNotesDestroy(notes);
    return status == UMI_STATUS_OK ? EXIT_SUCCESS : EXIT_FAILURE;
}
