/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/federated_round.c
 *
 * PURPOSE:
 *   Compute failure-atomic, sample-weighted FedAvg model deltas for
 *   compatible, independently verified worker updates using bounded native
 *   C23 arithmetic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/federated_round.h"

#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Conservatively reject overlapping output regions and uintptr_t wrap. */
static int RegionsOverlap(const void *first, size_t firstBytes,
                          const void *second, size_t secondBytes)
{
    uintptr_t a = (uintptr_t)first;
    uintptr_t b = (uintptr_t)second;
    if (a > UINTPTR_MAX - firstBytes || b > UINTPTR_MAX - secondBytes)
        return 1;
    return a < b + secondBytes && b < a + firstBytes;
}

struct UmiAiTrainingFederatedRound {
    UmiAiTrainingFederatedRoundConfig config;
    size_t accepted;
    uint64_t sampleTotal;
    double largestNorm;
    int finalised;
    double *weightedSum;
    double *candidateSum;
    float *candidateOutput;
    uint8_t identifiers[UMI_AI_FEDERATED_MAX_CONTRIBUTORS]
                       [UMI_AI_FEDERATED_DIGEST_BYTES];
};

static int DigestNonzero(const uint8_t bytes[UMI_AI_FEDERATED_DIGEST_BYTES])
{
    unsigned int bits = 0U;
    for (size_t i = 0U; i < UMI_AI_FEDERATED_DIGEST_BYTES; ++i)
        bits |= (unsigned int)bytes[i];
    return bits != 0U;
}

static UmiAiTrainingFederatedRoundReport Report(
    const UmiAiTrainingFederatedRound *round)
{
    UmiAiTrainingFederatedRoundReport report = {0};
    report.acceptedContributors = round->accepted;
    report.parameterCount = round->config.parameterCount;
    report.totalDistinctSamples = round->sampleTotal;
    report.maximumObservedDeltaL2Norm = round->largestNorm;
    report.finalised = round->finalised;
    return report;
}

UmiStatus UmiAiTrainingFederatedRoundCreate(
    const UmiAiTrainingFederatedRoundConfig *config,
    UmiAiTrainingFederatedRound **outRound)
{
    if (config == NULL || outRound == NULL || *outRound != NULL ||
        config->parameterCount == 0U ||
        config->parameterCount > UMI_AI_FEDERATED_MAX_PARAMETERS ||
        config->maximumContributors == 0U ||
        config->maximumContributors > UMI_AI_FEDERATED_MAX_CONTRIBUTORS ||
        !isfinite(config->maximumDeltaL2Norm) ||
        config->maximumDeltaL2Norm <= 0.0F ||
        !DigestNonzero(config->baseCheckpointSha256))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiAiTrainingFederatedRound *created = calloc(1U, sizeof(*created));
    if (created == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    created->config = *config;
    const size_t n = config->parameterCount;
    created->weightedSum = calloc(n, sizeof(double));
    created->candidateSum = calloc(n, sizeof(double));
    created->candidateOutput = calloc(n, sizeof(float));
    if (created->weightedSum == NULL || created->candidateSum == NULL ||
        created->candidateOutput == NULL) {
        UmiAiTrainingFederatedRoundDestroy(created);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    *outRound = created;
    return UMI_STATUS_OK;
}

void UmiAiTrainingFederatedRoundDestroy(UmiAiTrainingFederatedRound *round)
{
    if (round == NULL) return;
    /* Best-effort zeroing, not a promise of compiler-resistant secure erase. */
    if (round->weightedSum != NULL)
        memset(round->weightedSum, 0,
               round->config.parameterCount * sizeof(double));
    if (round->candidateSum != NULL)
        memset(round->candidateSum, 0,
               round->config.parameterCount * sizeof(double));
    if (round->candidateOutput != NULL)
        memset(round->candidateOutput, 0,
               round->config.parameterCount * sizeof(float));
    free(round->weightedSum);
    free(round->candidateSum);
    free(round->candidateOutput);
    memset(round->identifiers, 0, sizeof(round->identifiers));
    free(round);
}

UmiStatus UmiAiTrainingFederatedRoundSubmit(
    UmiAiTrainingFederatedRound *round,
    const UmiAiTrainingFederatedContribution *contribution)
{
    if (round == NULL || contribution == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (round->finalised) return UMI_STATUS_INVALID_STATE;
    if (round->accepted == round->config.maximumContributors)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (contribution->weightSpaceDelta == NULL ||
        contribution->deltaCount != round->config.parameterCount ||
        contribution->distinctSampleCount == 0U ||
        !DigestNonzero(contribution->contributorId) ||
        !DigestNonzero(contribution->datasetCommitmentSha256))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (memcmp(contribution->baseCheckpointSha256,
               round->config.baseCheckpointSha256,
               UMI_AI_FEDERATED_DIGEST_BYTES) != 0)
        return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < round->accepted; ++i)
        if (memcmp(round->identifiers[i], contribution->contributorId,
                   UMI_AI_FEDERATED_DIGEST_BYTES) == 0)
            return UMI_STATUS_ALREADY_EXISTS;
    if (contribution->distinctSampleCount > UINT64_MAX - round->sampleTotal)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    const size_t n = round->config.parameterCount;
    double squaredNorm = 0.0;
    for (size_t i = 0U; i < n; ++i) {
        const double value = (double)contribution->weightSpaceDelta[i];
        if (!isfinite(value)) return UMI_STATUS_INVALID_ARGUMENT;
        squaredNorm += value * value;
        if (!isfinite(squaredNorm)) return UMI_STATUS_INVALID_ARGUMENT;
        const double updated = round->weightedSum[i] +
            (double)contribution->distinctSampleCount * value;
        if (!isfinite(updated)) return UMI_STATUS_CAPACITY_EXCEEDED;
        round->candidateSum[i] = updated;
    }
    const double norm = sqrt(squaredNorm);
    if (!isfinite(norm) ||
        norm > (double)round->config.maximumDeltaL2Norm)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* The trial is complete: publish all sums and metadata as one operation. */
    memcpy(round->weightedSum, round->candidateSum, n * sizeof(double));
    memcpy(round->identifiers[round->accepted],
           contribution->contributorId, UMI_AI_FEDERATED_DIGEST_BYTES);
    ++round->accepted;
    round->sampleTotal += contribution->distinctSampleCount;
    if (norm > round->largestNorm) round->largestNorm = norm;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingFederatedRoundFinalise(
    UmiAiTrainingFederatedRound *round,
    float *outMeanDelta, size_t capacity,
    UmiAiTrainingFederatedRoundReport *outReport)
{
    if (round == NULL || outMeanDelta == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (capacity < round->config.parameterCount)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (outReport != NULL &&
        RegionsOverlap(outMeanDelta, round->config.parameterCount * sizeof(float),
                       outReport, sizeof(*outReport)))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (round->accepted == 0U || round->sampleTotal == 0U)
        return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < round->config.parameterCount; ++i) {
        const double mean = round->weightedSum[i] / (double)round->sampleTotal;
        if (!isfinite(mean) || mean > (double)FLT_MAX ||
            mean < -(double)FLT_MAX)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        round->candidateOutput[i] = (float)mean;
    }
    /* We publish data first, then the optional report. Neither reports a
     * cryptographically verified update, nor changes the underlying model. */
    memcpy(outMeanDelta, round->candidateOutput,
           round->config.parameterCount * sizeof(float));
    round->finalised = 1;
    if (outReport != NULL) *outReport = Report(round);
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingFederatedRoundInspect(
    const UmiAiTrainingFederatedRound *round,
    UmiAiTrainingFederatedRoundReport *outReport)
{
    if (round == NULL || outReport == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outReport = Report(round);
    return UMI_STATUS_OK;
}
