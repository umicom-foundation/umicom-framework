/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/build_review/notes_project/main.c
 *
 * PURPOSE:
 *   Count words in one explicit command-line note using the practice library.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "notes.h"
#include <stdio.h>
#include <string.h>
int main(int argc, char **argv)
{
    size_t words = 0U;
    if (argc != 2) { fputs("Usage: umicom-notes-practice \"your practice note\"\n", stderr); return 2; }
    if (NotesWordCount(argv[1], strlen(argv[1]), &words) != 0) return 1;
    printf("Words in note: %zu\n", words);
    return 0;
}
