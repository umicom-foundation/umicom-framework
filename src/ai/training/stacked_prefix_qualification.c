/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/stacked_prefix_qualification.c
 * PURPOSE: Compare real stacked-decoder causal prefix logits under two
 *          distinct future suffixes, without duplicating neural mathematics.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/stacked_prefix_qualification.h"
#include "umicom/ai/training/byte_tokenizer.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#define UMI_STACKED_PREFIX_CAUSAL_TOLERANCE 0.00004F

UmiStatus UmiAiTrainingStackedPrefixCausalityQualify(
    UmiAiTrainingStackedDecoder *model,
    const size_t *prefixTokenIds, size_t prefixTokenCount,
    UmiAiTrainingStackedPrefixQualification *outEvidence)
{
    UmiAiTrainingStackedDecoderInfo info = {0};
    UmiAiTrainingStackedModelIdentity pinned = {0};
    UmiAiTrainingStackedPrefixQualification candidate = {0};
    size_t contextBOS[UMI_AI_STACKED_MAX_CONTEXT] = {0};
    size_t contextByte[UMI_AI_STACKED_MAX_CONTEXT] = {0};
    float *reference = NULL, *alternative = NULL;
    UmiStatus status;
    if (model == NULL || prefixTokenIds == NULL || outEvidence == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingStackedDecoderInspect(model, &info);
    if (status != UMI_STATUS_OK) return status;
    const size_t context = (size_t)info.config.contextLength;
    const size_t vocabulary = (size_t)info.config.vocabularySize;
    if (context < 2U || context > UMI_AI_STACKED_MAX_CONTEXT ||
        prefixTokenCount == 0U || prefixTokenCount >= context ||
        vocabulary <= UMI_AI_BYTE_OFFSET ||
        vocabulary > UMI_AI_STACKED_MAX_VOCABULARY ||
        info.config.transformerLayers == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < prefixTokenCount; ++i)
        if (prefixTokenIds[i] >= vocabulary)
            return UMI_STATUS_INVALID_ARGUMENT;

    status = UmiAiTrainingStackedModelIdentityCapture(model, &pinned);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < context; ++i) {
        contextBOS[i] = i < prefixTokenCount ? prefixTokenIds[i] :
                         UMI_AI_BYTE_BOS;
        contextByte[i] = i < prefixTokenCount ? prefixTokenIds[i] :
                          UMI_AI_BYTE_OFFSET;
    }
    const size_t count = context * vocabulary;
    reference = malloc(count * sizeof(*reference));
    alternative = malloc(count * sizeof(*alternative));
    if (reference == NULL || alternative == NULL) {
        free(reference);
        free(alternative);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    status = UmiAiTrainingStackedDecoderLogits(
        model, contextBOS, context, reference, count);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingStackedDecoderLogits(
            model, contextByte, context, alternative, count);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingStackedModelIdentityVerify(model, &pinned);
    if (status == UMI_STATUS_OK) {
        float maximum = 0.0F;
        for (size_t row = 0U; row < prefixTokenCount && status == UMI_STATUS_OK;
             ++row) {
            for (size_t token = 0U; token < vocabulary; ++token) {
                size_t i = row * vocabulary + token;
                if (!isfinite(reference[i]) || !isfinite(alternative[i])) {
                    status = UMI_STATUS_INVALID_STATE;
                    break;
                }
                float delta = fabsf(reference[i] - alternative[i]);
                if (!isfinite(delta) || delta > UMI_STACKED_PREFIX_CAUSAL_TOLERANCE) {
                    status = UMI_STATUS_INVALID_STATE;
                    break;
                }
                if (delta > maximum) maximum = delta;
            }
        }
        if (status == UMI_STATUS_OK) {
            candidate.contextTokens = context;
            candidate.unchangedPrefixTokens = prefixTokenCount;
            candidate.comparedLogits = prefixTokenCount * vocabulary;
            candidate.transformerLayers = (size_t)info.config.transformerLayers;
            candidate.maximumAbsoluteDifference = maximum;
            candidate.modelIdentity = pinned;
        }
    }
    free(alternative);
    free(reference);
    if (status == UMI_STATUS_OK) *outEvidence = candidate;
    return status;
}
