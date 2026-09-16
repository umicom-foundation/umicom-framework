/*-----------------------------------------------------------------------------
 * Umicom Framework — application development lessons
 * File: examples/learning/projects/notes_model.h
 * PURPOSE: Compose document, view-model and command contracts for the Notes lessons.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LEARNING_NOTES_MODEL_H
#define UMICOM_LEARNING_NOTES_MODEL_H
#include "umicom/base/status.h"
#include "umicom/document/coordinator.h"

/* An opaque handle: the GTK adapter does not own the store or its text. */
typedef struct UmiLearningNotes UmiLearningNotes;
UmiStatus UmiLearningNotesCreate(UmiLearningNotes **outNotes);
void UmiLearningNotesDestroy(UmiLearningNotes *notes);
UmiStatus UmiLearningNotesSetText(UmiLearningNotes *notes, const char *text);
/* Allocates a copy for the caller. Release it with umi_document_store_free_text. */
UmiStatus UmiLearningNotesCopyText(UmiLearningNotes *notes, char **outText);
UmiStatus UmiLearningNotesSnapshot(UmiLearningNotes *notes, UmiDocumentWorkingCopySnapshot *outState);
UmiStatus UmiLearningNotesExecute(UmiLearningNotes *notes, const char *command, const char *argument);
#endif
