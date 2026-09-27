/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/build_review/notes_project/notes.c
 *
 * PURPOSE:
 *   Implement a bounded word count without changing caller-owned note text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "notes.h"
#include <string.h>
int NotesWordCount(const char *text, size_t length, size_t *outCount)
{
    size_t count = 0U;
    int inWord = 0;
    if (text == NULL || outCount == NULL || memchr(text, 0, length) != NULL) return 1;
    for (size_t i = 0U; i < length; ++i) {
        int space = text[i] == ' ' || text[i] == '\t' || text[i] == '\r' || text[i] == '\n';
        if (!space && !inWord) ++count;
        inWord = !space;
    }
    *outCount = count;
    return 0;
}
