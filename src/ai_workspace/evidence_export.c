/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_workspace/evidence_export.c
 * PURPOSE: Use the shared no-overwrite writer for frozen AI inspection reports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai_workspace/evidence_export.h"
#include <stdlib.h>

UmiStatus UmiAiEvidenceWriteNew(const UmiAiEvidenceReview *review, const char *path,
    UmiOutputFileSnapshot *outFile)
{
    if (review == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiOutputFileValidatePath(path);
    if (status != UMI_STATUS_OK) return status;
    /* A formatting or allocation failure must not leave an empty destination.
     * The formatter is already shared with the native inspection view. */
    char *text = malloc(UMI_AI_EVIDENCE_REPORT_CAPACITY);
    if (text == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    size_t length = 0U;
    status = UmiAiEvidenceFormat(review, text, UMI_AI_EVIDENCE_REPORT_CAPACITY, &length);
    UmiOutputFile *file = NULL;
    if (status == UMI_STATUS_OK) status = UmiOutputFileCreate(path, &file);
    if (status == UMI_STATUS_OK) {
        status = UmiOutputFileWrite(file, text, length);
        /* Close even after a partial write, retaining the first failure. The
         * snapshot describes real accepted bytes rather than the intended size. */
        UmiStatus closeStatus = UmiOutputFileClose(file);
        if (status == UMI_STATUS_OK) status = closeStatus;
        if (outFile != NULL) {
            UmiStatus readStatus = UmiOutputFileRead(file, outFile);
            if (status == UMI_STATUS_OK) status = readStatus;
        }
    }
    UmiOutputFileDestroy(file);
    free(text);
    return status;
}
