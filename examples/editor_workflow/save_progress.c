/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/editor_workflow/save_progress.c
 * PURPOSE:
 *   Read and explain a copied Notes Save All result through the public library.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/save_session.h"
#include <stdio.h>
#include <string.h>

/* Print only a result whose counters and state agree. The caller owns both
 * the snapshot and output buffer; neither function retains their addresses. */
static int ShowProgress(const UmiDocumentSaveProgress *progress)
{
    char message[UMI_DOCUMENT_NAME_CAPACITY + 384U];
    UmiStatus status = UmiDocumentSaveProgressValidate(progress);
    if (status == UMI_STATUS_OK)
        status = UmiDocumentSaveProgressFormat(progress, message, sizeof(message));
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Cannot display save progress: %s\n", umi_status_text(status));
        return 1;
    }
    return puts(message) == EOF ? 1 : 0;
}

int main(void)
{
    /* This is sample progress for teaching, not an instruction to save files.
     * notes.c was saved by this run, notes.h was saved separately, and the test
     * source remains pending. Production values come from the save session. */
    UmiDocumentSaveProgress progress = {0};
    progress.phase = UMI_DOCUMENT_SAVE_READY;
    progress.last_status = UMI_STATUS_OK;
    progress.total = 3U;
    progress.saved = 1U;
    progress.unchanged = 1U;
    progress.remaining = 1U;
    if (ShowProgress(&progress) != 0) return 1;

    progress.phase = UMI_DOCUMENT_SAVE_CANCELLED;
    progress.last_status = UMI_STATUS_CANCELLED;
    if (ShowProgress(&progress) != 0) return 1;

    /* A contradictory result must not replace an existing status message. */
    char message[128] = "Keep the previous status";
    progress.remaining = 0U;
    if (UmiDocumentSaveProgressFormat(&progress, message, sizeof(message)) !=
            UMI_STATUS_INVALID_ARGUMENT ||
        strcmp(message, "Keep the previous status") != 0) return 1;
    puts("Invalid progress was rejected; the previous status was preserved.");
    return 0;
}
