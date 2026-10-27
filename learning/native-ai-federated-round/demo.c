/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: learning/native-ai-federated-round/demo.c
 *
 * PURPOSE:
 *   Demonstrate native three-node sample-weighted model-delta aggregation
 *   without training or transport.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/federated_round.h"
#include <stdio.h>
#include <string.h>
int main(void)
{
    UmiAiTrainingFederatedRoundConfig config = {0};
    UmiAiTrainingFederatedRound *round = NULL;
    config.parameterCount = 1U;
    config.maximumContributors = 3U;
    config.maximumDeltaL2Norm = 2.0F;
    config.baseCheckpointSha256[0] = 0x55U;
    if (UmiAiTrainingFederatedRoundCreate(&config, &round) != UMI_STATUS_OK)
        return 1;
    const uint64_t samples[3] = {100U, 250U, 150U};
    const float deltas[3] = {0.05F, -0.01F, 0.08F};
    for (size_t i = 0U; i < 3U; ++i) {
        UmiAiTrainingFederatedContribution c = {0};
        c.contributorId[0] = (uint8_t)(i + 1U);
        c.datasetCommitmentSha256[0] = (uint8_t)(i + 21U);
        memcpy(c.baseCheckpointSha256, config.baseCheckpointSha256, 32U);
        c.distinctSampleCount = samples[i];
        c.weightSpaceDelta = &deltas[i];
        c.deltaCount = 1U;
        if (UmiAiTrainingFederatedRoundSubmit(round, &c) != UMI_STATUS_OK) {
            UmiAiTrainingFederatedRoundDestroy(round);
            return 1;
        }
    }
    float output = 0.0F;
    UmiAiTrainingFederatedRoundReport report = {0};
    const UmiStatus status = UmiAiTrainingFederatedRoundFinalise(round,
        &output, 1U, &report);
    if (status == UMI_STATUS_OK)
        printf("Synthetic FedAvg delta: %.6f; contributors: %zu; samples: %llu\n",
            (double)output, report.acceptedContributors,
            (unsigned long long)report.totalDistinctSamples);
    UmiAiTrainingFederatedRoundDestroy(round);
    return status == UMI_STATUS_OK ? 0 : 1;
}
