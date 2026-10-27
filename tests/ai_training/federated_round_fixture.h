/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/federated_round_fixture.h
 *
 * PURPOSE:
 *   Share deterministic synthetic contribution helpers between native
 *   federated aggregation tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_FEDERATED_ROUND_FIXTURE_H
#define UMICOM_AI_FEDERATED_ROUND_FIXTURE_H
#include "umicom/ai/training/federated_round.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(expression) do { if (!(expression)) { \
    fprintf(stderr, "test line %d: %s\n", __LINE__, #expression); \
    return 1; } } while (0)
static inline UmiAiTrainingFederatedRoundConfig FixtureConfig(
    size_t parameters, size_t maximumWorkers)
{
    UmiAiTrainingFederatedRoundConfig config = {0};
    config.parameterCount = parameters;
    config.maximumContributors = maximumWorkers;
    config.maximumDeltaL2Norm = 10.0F;
    config.baseCheckpointSha256[0] = 0xa5U;
    return config;
}
static inline UmiAiTrainingFederatedContribution FixtureContribution(
    const UmiAiTrainingFederatedRoundConfig *config, uint8_t contributor,
    uint64_t samples, const float *delta, size_t count)
{
    UmiAiTrainingFederatedContribution contribution = {0};
    contribution.contributorId[0] = contributor;
    contribution.datasetCommitmentSha256[0] = (uint8_t)(contributor + 20U);
    memcpy(contribution.baseCheckpointSha256, config->baseCheckpointSha256,
           UMI_AI_FEDERATED_DIGEST_BYTES);
    contribution.distinctSampleCount = samples;
    contribution.weightSpaceDelta = delta;
    contribution.deltaCount = count;
    return contribution;
}
#endif
