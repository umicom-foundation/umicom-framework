/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/stacked_generation_evidence.c
 * PURPOSE: SHA-256 evidence over independently repeated stacked generation.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/stacked_generation_evidence.h"

#include <string.h>

static UmiStatus HashTokenIds(
    const char *domain, const size_t *tokens, size_t count,
    uint8_t digest[UMI_SHA256_BYTES])
{
    UmiSha256 hash;
    uint8_t packed[8];
    UmiStatus status;
    if (domain == NULL || digest == NULL ||
        (count > 0U && tokens == NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiSha256Init(&hash);
    status = UmiSha256Update(&hash, domain, strlen(domain));
    for (size_t i = 0U; i < sizeof(packed) && status == UMI_STATUS_OK; ++i)
        packed[i] = (uint8_t)((uint64_t)count >> (8U * i));
    if (status == UMI_STATUS_OK)
        status = UmiSha256Update(&hash, packed, sizeof(packed));
    for (size_t t = 0U; t < count && status == UMI_STATUS_OK; ++t) {
        for (size_t i = 0U; i < sizeof(packed); ++i)
            packed[i] = (uint8_t)((uint64_t)tokens[t] >> (8U * i));
        status = UmiSha256Update(&hash, packed, sizeof(packed));
    }
    return status == UMI_STATUS_OK
        ? UmiSha256Final(&hash, digest) : status;
}

/* A canonical policy digest detects accidental changes to the sampler
 * declaration. It does not prove who approved the policy or the model. */
static UmiStatus HashSamplingPolicy(
    const UmiAiTrainingNucleusOptions *policy, uint64_t seed,
    uint8_t digest[UMI_SHA256_BYTES])
{
    static const char domain[] = "umicom.stacked.generation.policy\n";
    UmiSha256 hash;
    uint8_t packed[8] = {0};
    uint32_t temperatureBits = 0U, topPBits = 0U;
    UmiStatus status;
    if (policy == NULL || digest == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memcpy(&temperatureBits, &policy->temperature, sizeof(temperatureBits));
    memcpy(&topPBits, &policy->topP, sizeof(topPBits));
    UmiSha256Init(&hash);
    status = UmiSha256Update(&hash, domain, sizeof(domain) - 1U);
    for (size_t i = 0U; i < 4U && status == UMI_STATUS_OK; ++i)
        packed[i] = (uint8_t)(temperatureBits >> (8U * i));
    if (status == UMI_STATUS_OK)
        status = UmiSha256Update(&hash, packed, 4U);
    for (size_t i = 0U; i < sizeof(packed) && status == UMI_STATUS_OK; ++i)
        packed[i] = (uint8_t)((uint64_t)policy->topK >> (8U * i));
    if (status == UMI_STATUS_OK)
        status = UmiSha256Update(&hash, packed, sizeof(packed));
    for (size_t i = 0U; i < 4U && status == UMI_STATUS_OK; ++i)
        packed[i] = (uint8_t)(topPBits >> (8U * i));
    if (status == UMI_STATUS_OK)
        status = UmiSha256Update(&hash, packed, 4U);
    for (size_t i = 0U; i < sizeof(packed) && status == UMI_STATUS_OK; ++i)
        packed[i] = (uint8_t)(seed >> (8U * i));
    if (status == UMI_STATUS_OK)
        status = UmiSha256Update(&hash, packed, sizeof(packed));
    return status == UMI_STATUS_OK
        ? UmiSha256Final(&hash, digest) : status;
}

UmiStatus UmiAiTrainingStackedGenerationEvidenceBuild(
    UmiAiTrainingStackedDecoder *model,
    const size_t *promptIds, size_t promptCount,
    const UmiAiTrainingNucleusOptions *policy,
    uint64_t seed, const size_t *claimedTokens, size_t claimedCount,
    UmiAiTrainingStackedGenerationEvidence *outEvidence)
{
    UmiAiTrainingStackedGenerationEvidence evidence = {0};
    size_t computed[UMI_AI_STACKED_MAX_GENERATED_TOKENS] = {0};
    size_t computedCount = 0U;
    UmiStatus status;
    if (model == NULL || policy == NULL || outEvidence == NULL ||
        claimedTokens == NULL || claimedCount == 0U ||
        claimedCount > UMI_AI_STACKED_MAX_GENERATED_TOKENS ||
        promptCount > UMI_AI_STACKED_INFERENCE_MAX_PROMPT_TOKENS ||
        (promptCount != 0U && promptIds == NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingStackedGenerateNucleus(model, promptIds,
        promptCount, policy, claimedCount, seed,
        computed, UMI_AI_STACKED_MAX_GENERATED_TOKENS,
        &computedCount, &evidence.generation);
    if (status != UMI_STATUS_OK) return status;
    if (computedCount != claimedCount ||
        memcmp(computed, claimedTokens, claimedCount * sizeof(*computed)) != 0)
        return UMI_STATUS_INVALID_STATE;
    status = HashSamplingPolicy(policy, seed, evidence.samplingPolicySha256);
    if (status != UMI_STATUS_OK) return status;
    status = HashTokenIds("umicom.stacked.prompt\n", promptIds,
                          promptCount, evidence.orderedPromptSha256);
    if (status != UMI_STATUS_OK) return status;
    status = HashTokenIds("umicom.stacked.output\n", claimedTokens,
                          claimedCount, evidence.orderedOutputSha256);
    if (status != UMI_STATUS_OK) return status;
    evidence.promptTokenCount = promptCount;
    evidence.outputTokenCount = claimedCount;
    evidence.policy = *policy;
    evidence.seed = seed;
    *outEvidence = evidence;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingStackedGenerationEvidenceVerify(
    UmiAiTrainingStackedDecoder *model,
    const size_t *promptIds, size_t promptCount,
    const size_t *claimedTokens, size_t claimedCount,
    const UmiAiTrainingStackedGenerationEvidence *expected)
{
    UmiAiTrainingStackedGenerationEvidence actual = {0};
    UmiStatus status;
    if (expected == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (expected->promptTokenCount != promptCount ||
        expected->outputTokenCount != claimedCount)
        return UMI_STATUS_INVALID_STATE;
    status = UmiAiTrainingStackedGenerationEvidenceBuild(
        model, promptIds, promptCount, &expected->policy,
        expected->seed, claimedTokens, claimedCount, &actual);
    if (status != UMI_STATUS_OK) return status;
    return actual.generation.generatedTokens ==
               expected->generation.generatedTokens &&
        actual.generation.contextTokens == expected->generation.contextTokens &&
        actual.generation.transformerLayers == expected->generation.transformerLayers &&
        actual.generation.computedFullContextTokenRows ==
               expected->generation.computedFullContextTokenRows &&
        actual.generation.computedTransformerTokenLayerRows ==
               expected->generation.computedTransformerTokenLayerRows &&
        actual.generation.modelIdentity.checkpointBytes ==
               expected->generation.modelIdentity.checkpointBytes &&
        memcmp(actual.generation.modelIdentity.checkpointSha256,
               expected->generation.modelIdentity.checkpointSha256,
               UMI_SHA256_BYTES) == 0 &&
        actual.generation.modelIdentity.completedTrainingSteps ==
               expected->generation.modelIdentity.completedTrainingSteps &&
        memcmp(actual.samplingPolicySha256, expected->samplingPolicySha256,
               UMI_SHA256_BYTES) == 0 &&
        memcmp(actual.orderedPromptSha256, expected->orderedPromptSha256,
               UMI_SHA256_BYTES) == 0 &&
        memcmp(actual.orderedOutputSha256, expected->orderedOutputSha256,
               UMI_SHA256_BYTES) == 0
        ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}
