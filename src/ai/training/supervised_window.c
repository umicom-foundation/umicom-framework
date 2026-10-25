/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/supervised_window.c
 * PURPOSE:
 *   Build length-bounded assistant-response token targets from reviewed bytes
 *   using the original Framework byte tokenizer and reserved token IDs.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/supervised_window.h"

#include <string.h>

UmiStatus UmiAiTrainingSupervisedByteWindowBuild(
    const uint8_t *promptBytes, size_t promptLength,
    const uint8_t *completionBytes, size_t completionLength,
    size_t contextLength,
    size_t *outInputs, size_t inputCapacity,
    size_t *outTargets, size_t targetCapacity,
    uint8_t *outLossMask, size_t maskCapacity,
    UmiAiTrainingSupervisedWindowInfo *outInfo)
{
    uint32_t encodedPrompt[UMI_AI_DECODER_MAX_CONTEXT] = {0};
    uint32_t encodedCompletion[UMI_AI_DECODER_MAX_CONTEXT] = {0};
    size_t stagedInputs[UMI_AI_DECODER_MAX_CONTEXT] = {0};
    size_t stagedTargets[UMI_AI_DECODER_MAX_CONTEXT] = {0};
    uint8_t stagedMask[UMI_AI_DECODER_MAX_CONTEXT] = {0};
    UmiAiTrainingSupervisedWindowInfo info = {0};
    size_t encodedCount = 0U;
    UmiStatus status;

    if (promptBytes == NULL || completionBytes == NULL ||
        outInputs == NULL || outTargets == NULL || outLossMask == NULL ||
        outInfo == NULL || outInputs == outTargets ||
        promptLength == 0U || completionLength == 0U ||
        contextLength == 0U || contextLength > UMI_AI_DECODER_MAX_CONTEXT)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (inputCapacity < contextLength || targetCapacity < contextLength ||
        maskCapacity < contextLength ||
        promptLength >= contextLength ||
        completionLength > contextLength - promptLength - 1U)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    /* The established tokenizer owns byte -> token ID meaning. Never copy
     * its private byte vocabulary or invent a second special-token scheme. */
    status = UmiAiByteEncode(promptBytes, promptLength,
                             encodedPrompt, UMI_AI_DECODER_MAX_CONTEXT,
                             &encodedCount);
    if (status != UMI_STATUS_OK) return status;
    if (encodedCount != promptLength) return UMI_STATUS_INVALID_STATE;
    status = UmiAiByteEncode(completionBytes, completionLength,
                             encodedCompletion, UMI_AI_DECODER_MAX_CONTEXT,
                             &encodedCount);
    if (status != UMI_STATUS_OK) return status;
    if (encodedCount != completionLength) return UMI_STATUS_INVALID_STATE;

    /* The stream includes BOS and EOS. Stage it without a dynamically sized
     * allocation; the decoder already bounds its educational context to 32. */
    size_t sequence[UMI_AI_DECODER_MAX_CONTEXT + 2U] = {0};
    size_t length = 0U;
    sequence[length++] = UMI_AI_BYTE_BOS;
    for (size_t i = 0U; i < promptLength; ++i)
        sequence[length++] = encodedPrompt[i];
    for (size_t i = 0U; i < completionLength; ++i)
        sequence[length++] = encodedCompletion[i];
    sequence[length++] = UMI_AI_BYTE_EOS;

    /* A target begins at the very next position, so the target for the last
     * prompt token is the FIRST completion byte and must be supervised. */
    for (size_t row = 0U; row < contextLength; ++row) {
        stagedInputs[row] = row < length ? sequence[row] : UMI_AI_BYTE_PAD;
        stagedTargets[row] = row + 1U < length ? sequence[row + 1U]
                                                : UMI_AI_BYTE_PAD;
        stagedMask[row] = (row >= promptLength &&
                           row < promptLength + completionLength + 1U)
                           ? 1U : 0U;
    }

    info.contextTokens = contextLength;
    info.promptBytes = promptLength;
    info.completionBytes = completionLength;
    info.supervisedTargets = completionLength + 1U;
    info.ignoredTargets = contextLength - info.supervisedTargets;

    /* Commit only when the complete next-token window has been prepared. */
    (void)memcpy(outInputs, stagedInputs, contextLength * sizeof(*outInputs));
    (void)memcpy(outTargets, stagedTargets, contextLength * sizeof(*outTargets));
    (void)memcpy(outLossMask, stagedMask, contextLength);
    *outInfo = info;
    return UMI_STATUS_OK;
}
