/*-----------------------------------------------------------------------------
 * Umicom Framework — application development lessons
 * File: examples/learning/projects/work_log.c
 * PURPOSE: Write a small activity report for a Umicom application.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>

int main(void)
{
    int savedNotes = 3;
    int pendingNotes = 1;

    puts("Umicom Notes — activity report");
    printf("Saved notes: %d\n", savedNotes);
    printf("Notes waiting to be saved: %d\n", pendingNotes);
    return 0;
}
