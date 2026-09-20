/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document_saving/test_save_link_gtk4.c
 * PURPOSE:
 *   Force the native linker to resolve the public save-progress accessor and
 *   formatter, then exercise their safe argument boundaries without a display.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/document_commands.h"
#include <stdio.h>
#include <string.h>

int main(void)
{
    UmiDocumentSaveProgress progress;
    memset(&progress, 0x5a, sizeof(progress));
    UmiDocumentSaveProgress before;
    memcpy(&before, &progress, sizeof(before));
    /* Unlike a _Generic signature check, these calls require definitions in
     * the actual linked libraries. Do not replace them with local stubs. */
    if (UmiGtk4AdapterDocumentSaveAllProgress(NULL, &progress) != UMI_STATUS_INVALID_ARGUMENT ||
        memcmp(&progress, &before, sizeof(progress)) != 0 ||
        UmiGtk4AdapterDocumentSaveAllProgress(NULL, NULL) != UMI_STATUS_INVALID_ARGUMENT)
        return 1;
    memset(&progress, 0, sizeof(progress));
    progress.phase = UMI_DOCUMENT_SAVE_COMPLETE;
    progress.last_status = UMI_STATUS_OK;
    char text[128];
    if (UmiDocumentSaveProgressValidate(&progress) != UMI_STATUS_OK ||
        UmiDocumentSaveProgressFormat(&progress, text, sizeof(text)) != UMI_STATUS_OK ||
        strcmp(text, "Save All complete: 0 saved, 0 already saved.") != 0)
        return 1;
    puts("Save progress: core and GTK symbols linked; null-argument boundaries passed.");
    return 0;
}
