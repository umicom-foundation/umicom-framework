/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/stacked_prefix_cache_plan.c
 * PURPOSE: Conditioned, overflow-checked multi-layer token-work forecast.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/stacked_prefix_cache_plan.h"

#include <stdint.h>

static int Multiply(uint64_t left, uint64_t right, uint64_t *out)
{
    if (left != 0U && right > UINT64_MAX / left) return 0;
    *out = left * right;
    return 1;
}
static int Add(uint64_t left, uint64_t right, uint64_t *out)
{
    if (right > UINT64_MAX - left) return 0;
    *out = left + right;
    return 1;
}

UmiStatus UmiAiTrainingStackedPrefixCachePlanBuild(
    const UmiAiTrainingStackedDecoderConfig *config,
    size_t promptTokens, size_t generatedTokens,
    UmiAiTrainingStackedPrefixCachePlan *outPlan)
{
    UmiAiTrainingModelScalingShape scaling;
    UmiAiTrainingModelScalingEstimate memory={0};
    UmiAiTrainingStackedPrefixCachePlan candidate={0};
    uint64_t added, reused, shifted, rows, candidateRows, fullRows;
    UmiStatus status;
    if(config==NULL || outPlan==NULL ||
       config->vocabularySize<5U || config->vocabularySize>UMI_AI_STACKED_MAX_VOCABULARY ||
       config->contextLength==0U || config->contextLength>UMI_AI_STACKED_MAX_CONTEXT ||
       config->channels<2U || config->channels>UMI_AI_STACKED_MAX_CHANNELS ||
       config->hiddenChannels==0U || config->hiddenChannels>UMI_AI_STACKED_MAX_HIDDEN_CHANNELS ||
       config->attentionHeads==0U || config->channels % config->attentionHeads!=0U ||
       config->transformerLayers==0U || config->transformerLayers>UMI_AI_STACKED_MAX_LAYERS ||
       promptTokens==0U || promptTokens>config->contextLength ||
       generatedTokens==0U || generatedTokens>UMI_AI_STACKED_MAX_GENERATED_TOKENS)
        return UMI_STATUS_INVALID_ARGUMENT;

    scaling=(UmiAiTrainingModelScalingShape){
        .vocabulary=config->vocabularySize,
        .contextTokens=config->contextLength,
        .channels=config->channels,
        .hiddenChannels=config->hiddenChannels,
        .attentionHeads=config->attentionHeads,
        .transformerLayers=config->transformerLayers,
        .concurrentSequences=1U,
        .adapterRank=0U
    };
    status=UmiAiTrainingModelScalingEstimateBuild(&scaling,&memory);
    if(status!=UMI_STATUS_OK)return status;

    /* One predicted token is a pending sample, not a completed Append.
     * First P positions are prefilling the growing prefix. */
    added=(uint64_t)generatedTokens-1U;
    reused=added;
    if(reused>(uint64_t)config->contextLength-(uint64_t)promptTokens)
        reused=(uint64_t)config->contextLength-(uint64_t)promptTokens;
    shifted=added-reused;
    if(!Multiply((uint64_t)config->contextLength,(uint64_t)generatedTokens,&rows) ||
       !Multiply(rows,(uint64_t)config->transformerLayers,&fullRows) ||
       !Multiply((uint64_t)config->contextLength,shifted,&rows) ||
       !Add(rows,reused,&candidateRows) ||
       !Add(candidateRows,(uint64_t)promptTokens,&candidateRows) ||
       !Multiply(candidateRows,(uint64_t)config->transformerLayers,&candidateRows) ||
       candidateRows>fullRows)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    candidate.contextTokens=config->contextLength;
    candidate.promptTokens=promptTokens;
    candidate.generatedTokens=generatedTokens;
    candidate.transformerLayers=config->transformerLayers;
    candidate.canonicalTokenLayerRows=fullRows;
    candidate.candidateTokenLayerRows=candidateRows;
    candidate.avoidedTokenLayerRows=fullRows-candidateRows;
    candidate.prefixSingleRowAppends=reused;
    candidate.fullWindowRebuilds=shifted;
    candidate.kvTensorFp32Bytes=memory.kvCacheFp32Bytes;
    *outPlan=candidate;
    return UMI_STATUS_OK;
}
