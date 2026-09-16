/*-----------------------------------------------------------------------------
 * Umicom Framework — application development lessons
 * File: examples/learning/projects/binary_flags.c
 * PURPOSE: Inspect independent display settings as binary flags.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define NOTES_SHOW_LINE_NUMBERS UINT32_C(1)
#define NOTES_WORD_WRAP UINT32_C(2)
#define NOTES_SHOW_PREVIEW UINT32_C(4)

static void PrintBinary(uint32_t value)
{
    for (unsigned position = 3U; position > 0U; --position)
        putchar((value & (UINT32_C(1) << (position - 1U))) != 0U ? '1' : '0');
    putchar('\n');
}

int main(void)
{
    uint32_t settings = NOTES_SHOW_LINE_NUMBERS | NOTES_WORD_WRAP;
    fputs("Umicom Notes settings: ", stdout);
    PrintBinary(settings);
    settings |= NOTES_SHOW_PREVIEW;
    settings &= ~NOTES_WORD_WRAP;
    fputs("Preview on, wrapping off: ", stdout);
    PrintBinary(settings);
    return settings == UINT32_C(5) ? EXIT_SUCCESS : EXIT_FAILURE;
}
