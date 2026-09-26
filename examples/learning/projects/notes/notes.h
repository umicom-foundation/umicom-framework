/* Umicom Framework teaching project: notes / notes.h
 * Author: Sammy Hegab, Umicom Foundation. Licence: MIT.
 * This inspectable lesson is not a replacement for production Framework services. */
#ifndef UMICOM_LESSON_NOTES_H
#define UMICOM_LESSON_NOTES_H
#include <stddef.h>
#define NOTE_CAPACITY 8U
#define NOTE_KEY_CAPACITY 64U
#define NOTE_TEXT_CAPACITY 128U
typedef struct Note {char key[NOTE_KEY_CAPACITY];char text[NOTE_TEXT_CAPACITY];} Note;
typedef struct NoteBook {Note entries[NOTE_CAPACITY];size_t count;} NoteBook;
/* Initialise before first use. All storage belongs to the caller's object. */
void NotesInit(NoteBook *book);
/* Copy successful input; on failure leave every existing entry unchanged.
 * 0: success, 1: invalid input, 2: capacity exceeded. Keys are nonempty text. */
int NotesSet(NoteBook *book,const char *key,const char *text);
/* Return borrowed text, invalidated by later mutation/destruction of book. */
const char *NotesFind(const NoteBook *book,const char *key);
#endif
