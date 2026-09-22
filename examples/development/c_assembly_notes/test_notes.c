/*-----------------------------------------------------------------------------
 * Umicom Framework — C and Assembly example
 * File: examples/development/c_assembly_notes/test_notes.c
 * PURPOSE: Compare C/Assembly behaviour with independently counted expectations.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "notes.h"
#include <stdio.h>
#include <string.h>

static int Check(const unsigned char *text, size_t length, size_t lines)
{
    UmiNotesStatistics actual = {0};
    if (!UmiNotesInspect(text, length, &actual) || actual.bytes != length || actual.lines != lines) {
        fprintf(stderr, "Unexpected statistics for %zu bytes.\n", length); return 1;
    }
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (strcmp(argv[1], "empty") == 0) return Check(NULL, 0U, 0U);
    if (strcmp(argv[1], "single-line") == 0) return Check((const unsigned char *)"Note", 4U, 1U);
    if (strcmp(argv[1], "lines") == 0) return Check((const unsigned char *)"One\nTwo\n", 8U, 2U);
    if (strcmp(argv[1], "unicode") == 0) {
        const unsigned char text[] = "\xCE\xBB \xF0\x9F\x9A\x80\nNote";
        return Check(text, sizeof(text) - 1U, 2U);
    }
    if (strcmp(argv[1], "binary") == 0) {
        const unsigned char text[] = {'A', 0, '\n', 0};
        return Check(text, sizeof(text), 2U);
    }
    if (strcmp(argv[1], "invalid") == 0) {
        UmiNotesStatistics output = {19U, 7U};
        return UmiNotesInspect(NULL, 1U, &output) || output.bytes != 19U || output.lines != 7U ||
            UmiNotesInspect((const unsigned char *)"x", 1U, NULL);
    }
    if (strcmp(argv[1], "reference") == 0) {
        unsigned char bytes[1024];
        for (size_t i = 0U; i < sizeof(bytes); ++i) bytes[i] = (unsigned char)(i % 251U);
        for (unsigned value = 0U; value < 256U; ++value) {
            for (size_t length = 0U; length <= sizeof(bytes); length += 16U) {
                size_t expected = 0U;
                for (size_t i = 0U; i < length; ++i) if (bytes[i] == value) ++expected;
                if (UmiNotesCountByte(bytes, length, (unsigned char)value) != expected) return 1;
            }
        }
        return 0;
    }
    return 2;
}
