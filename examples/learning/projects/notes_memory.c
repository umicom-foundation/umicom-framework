/*-----------------------------------------------------------------------------
 * Umicom Framework — application development lessons
 * File: examples/learning/projects/notes_memory.c
 * PURPOSE: Trace note ownership using the existing Framework document API.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/platform/document.h"

int main(void)
{
    UmiDocument note;
    char title[] = "Umicom Notes: review the build output.\n";
    umi_document_init(&note);
    UmiStatus status = umi_document_set_text(&note, title);
    /* Framework copied the text. Changing the input does not change the note. */
    title[0] = 'X';
    if (status == UMI_STATUS_OK)
        status = umi_document_append(&note, "Next: run the tests.\n");
    if (status == UMI_STATUS_OK) {
        printf("The document owns %zu text bytes.\n%s", note.length, note.text);
        if (strncmp(note.text, "Umicom", 6U) != 0) status = UMI_STATUS_INVALID_STATE;
    }
    umi_document_dispose(&note);
    return status == UMI_STATUS_OK ? EXIT_SUCCESS : EXIT_FAILURE;
}
