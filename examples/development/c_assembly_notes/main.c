/*-----------------------------------------------------------------------------
 * Umicom Framework — C and Assembly example
 * File: examples/development/c_assembly_notes/main.c
 * PURPOSE: Write a new note or inspect an existing note from the command line.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "notes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Refuse an existing destination. A lesson must not overwrite a user's draft. */
static int WriteNote(const char *path, const char *text)
{
    FILE *file = fopen(path, "wbx");
    if (file == NULL) { perror("Cannot create a new note"); return 1; }
    size_t length = strlen(text);
    int failed = fwrite(text, 1U, length, file) != length;
    if (fclose(file) != 0) failed = 1;
    if (failed) { fputs("The note was not completely written.\n", stderr); return 1; }
    printf("Created: %s\n", path);
    return 0;
}

static int InspectNote(const char *path)
{
    const size_t maximum = 1024U * 1024U;
    FILE *file = fopen(path, "rb");
    if (file == NULL) { perror("Cannot open the note"); return 1; }
    unsigned char *text = malloc(maximum + 1U);
    if (text == NULL) { fclose(file); return 1; }
    size_t length = fread(text, 1U, maximum + 1U, file);
    int failed = ferror(file) || length > maximum;
    if (fclose(file) != 0) failed = 1;
    UmiNotesStatistics statistics;
    if (!failed) failed = !UmiNotesInspect(text, length, &statistics);
    free(text);
    if (failed) { fputs("Could not read a complete note within the 1 MiB limit.\n", stderr); return 1; }
    printf("Bytes: %zu\nLines: %zu\n", statistics.bytes, statistics.lines);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc == 2 && argv[1][0] != '-') return InspectNote(argv[1]);
    if (argc == 4 && strcmp(argv[1], "--write") == 0) return WriteNote(argv[2], argv[3]);
    if (argc == 3 && strcmp(argv[1], "--inspect") == 0) return InspectNote(argv[2]);
    fputs("Umicom Notes\n  FILE\n  --write NEW_FILE TEXT\n  --inspect FILE\n", stderr);
    return 2;
}
