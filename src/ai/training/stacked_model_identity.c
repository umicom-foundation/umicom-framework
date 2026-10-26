/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/stacked_model_identity.c
 * PURPOSE: Reuse the canonical stacked checkpoint and base SHA-256 owners.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/stacked_model_identity.h"

#include <stdlib.h>
#include <string.h>

static int SameConfig(const UmiAiTrainingStackedDecoderConfig *a,
                      const UmiAiTrainingStackedDecoderConfig *b)
{
    return a->vocabularySize == b->vocabularySize &&
        a->contextLength == b->contextLength &&
        a->channels == b->channels &&
        a->hiddenChannels == b->hiddenChannels &&
        a->attentionHeads == b->attentionHeads &&
        a->transformerLayers == b->transformerLayers &&
        a->initialSeed == b->initialSeed;
}

UmiStatus UmiAiTrainingStackedModelIdentityCapture(
    const UmiAiTrainingStackedDecoder *model,
    UmiAiTrainingStackedModelIdentity *outIdentity)
{
    UmiAiTrainingStackedModelIdentity candidate = {0};
    UmiAiTrainingStackedDecoderInfo info = {0};
    UmiSha256 hash;
    uint8_t *checkpoint = NULL;
    size_t required = 0U, written = 0U;
    UmiStatus status;
    if (model == NULL || outIdentity == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingStackedDecoderInspect(model, &info);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingStackedCheckpointSize(model, &required);
    if (status != UMI_STATUS_OK) return status;
    if (required == 0U || required > UMI_AI_STACKED_CHECKPOINT_MAX_BYTES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    checkpoint = malloc(required);
    if (checkpoint == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = UmiAiTrainingStackedCheckpointExport(
        model, checkpoint, required, &written);
    if (status == UMI_STATUS_OK && written != required)
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) {
        UmiSha256Init(&hash);
        status = UmiSha256Update(&hash, checkpoint, written);
        if (status == UMI_STATUS_OK)
            status = UmiSha256Final(&hash, candidate.checkpointSha256);
    }
    free(checkpoint);
    if (status != UMI_STATUS_OK) return status;
    candidate.config = info.config;
    candidate.completedTrainingSteps = info.completedTrainingSteps;
    candidate.checkpointBytes = required;
    *outIdentity = candidate;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingStackedModelIdentityVerify(
    const UmiAiTrainingStackedDecoder *model,
    const UmiAiTrainingStackedModelIdentity *expected)
{
    UmiAiTrainingStackedModelIdentity observed = {0};
    UmiStatus status;
    if (model == NULL || expected == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingStackedModelIdentityCapture(model, &observed);
    if (status != UMI_STATUS_OK) return status;
    return SameConfig(&observed.config, &expected->config) &&
        observed.completedTrainingSteps == expected->completedTrainingSteps &&
        observed.checkpointBytes == expected->checkpointBytes &&
        memcmp(observed.checkpointSha256, expected->checkpointSha256,
               UMI_SHA256_BYTES) == 0
        ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}
