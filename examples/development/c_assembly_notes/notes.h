/*-----------------------------------------------------------------------------
 * Umicom Framework — C and Assembly example
 * File: examples/development/c_assembly_notes/notes.h
 * PURPOSE: Inspect byte and line counts through a small C-compatible contract.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_NOTES_STATISTICS_H
#define UMICOM_NOTES_STATISTICS_H
#include <stddef.h>

typedef struct UmiNotesStatistics {
    size_t bytes;
    size_t lines;
} UmiNotesStatistics;

/* A line ends at LF; a final unterminated line is counted too. UTF-8 bytes are
 * not characters. Return zero for invalid arguments without changing output. */
int UmiNotesInspect(const unsigned char *text, size_t length, UmiNotesStatistics *out);

/* Leaf assembly function. text may be NULL only when length is zero. */
size_t UmiNotesCountByte(const unsigned char *text, size_t length, unsigned char value);
#endif
