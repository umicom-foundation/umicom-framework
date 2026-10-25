/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/source_evidence.c
 * PURPOSE: Verify actual reviewed training-source bytes before admission.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/source_evidence.h"
#include "umicom/base/sha256.h"

/* Digest equality is deliberately compared as hex rather than carrying
 * source bytes or reviewer notes into an admission record. Uppercase hex is
 * accepted because the existing admission parser permits both cases. */
static int EvidenceHexEqual(const char *left, const char *right)
{
    unsigned differences = 0U;
    if (left == NULL || right == NULL) return 0;
    for (size_t i = 0U; i < 64U; ++i) {
        unsigned char l = (unsigned char)left[i];
        unsigned char r = (unsigned char)right[i];
        if (l >= 'A' && l <= 'F') l = (unsigned char)(l - 'A' + 'a');
        if (r >= 'A' && r <= 'F') r = (unsigned char)(r - 'A' + 'a');
        differences |= (unsigned)(l ^ r);
    }
    return differences == 0U && left[64] == '\0' && right[64] == '\0';
}

UmiStatus UmiAiTrainingSourceVerifyBytes(
    const UmiAiTrainingSourceReview *review,
    const void *originalBytes, size_t originalLength,
    const void *preparedTextBytes, size_t preparedTextLength,
    unsigned evaluationPercentage,
    UmiAiTrainingAdmittedSource *outSource)
{
    char actualOriginal[UMI_SHA256_HEX_CAPACITY];
    char actualPrepared[UMI_SHA256_HEX_CAPACITY];
    UmiAiTrainingAdmittedSource admitted = {0};
    UmiStatus status;
    if (review == NULL || outSource == NULL ||
        originalBytes == NULL || originalLength == 0U ||
        preparedTextBytes == NULL || preparedTextLength == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;

    /* Permissions are applied FIRST. An unreviewed file is never processed
     * merely to compute its digest on behalf of a rejected training request. */
    status = UmiAiTrainingSourceAdmit(review, evaluationPercentage, &admitted);
    if (status != UMI_STATUS_OK) return status;

    status = UmiSha256Buffer(originalBytes, originalLength, actualOriginal);
    if (status != UMI_STATUS_OK) return status;
    status = UmiSha256Buffer(preparedTextBytes, preparedTextLength, actualPrepared);
    if (status != UMI_STATUS_OK) return status;
    if (!EvidenceHexEqual(actualOriginal, admitted.originalAssetSha256) ||
        !EvidenceHexEqual(actualPrepared, admitted.preparedTextSha256))
        return UMI_STATUS_INVALID_STATE;

    *outSource = admitted;
    return UMI_STATUS_OK;
}
