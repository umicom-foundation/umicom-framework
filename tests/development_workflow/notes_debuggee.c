/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/development_workflow/notes_debuggee.c
 * PURPOSE: A real debug target with observable Notes values and a stable breakpoint.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
int main(void)
{
    int savedNotes = 2;
    int pendingNotes = 3;
    int totalNotes = savedNotes + pendingNotes; /* NOTES_BREAKPOINT */
    savedNotes = totalNotes;
    printf("Umicom Notes: %d saved\n", savedNotes);
    return savedNotes == 5 ? 0 : 1;
}
