/*-----------------------------------------------------------------------------
 * Umicom Framework — C and Assembly example
 * File: examples/development/c_assembly_notes/notes.c
 * PURPOSE: Keep line-count policy in C and call an inspectable assembly leaf.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "notes.h"

int UmiNotesInspect(const unsigned char *text, size_t length, UmiNotesStatistics *out)
{
    if (out == NULL || (text == NULL && length != 0U)) return 0;
    UmiNotesStatistics statistics = {length, 0U};
    statistics.lines = UmiNotesCountByte(text, length, (unsigned char)'\n');
    if (length != 0U && text[length - 1U] != (unsigned char)'\n') ++statistics.lines;
    *out = statistics;
    return 1;
}
