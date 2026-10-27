/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_federated_round_weighted.c
 *
 * PURPOSE:
 *   Validate three-worker sample-weighted FedAvg arithmetic and idempotent
 *   result retrieval.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "federated_round_fixture.h"
int main(void)
{
    UmiAiTrainingFederatedRoundConfig cfg = FixtureConfig(2U, 3U);
    UmiAiTrainingFederatedRound *round = NULL;
    const float a[2] = {0.05F, 0.20F};
    const float b[2] = {-0.01F, -0.04F};
    const float c[2] = {0.08F, 0.10F};
    float mean[2] = {55.0F, 55.0F};
    UmiAiTrainingFederatedRoundReport report = {0};
    CHECK(UmiAiTrainingFederatedRoundCreate(&cfg, &round) == UMI_STATUS_OK);
    UmiAiTrainingFederatedContribution ca = FixtureContribution(&cfg, 1U, 100U, a, 2U);
    UmiAiTrainingFederatedContribution cb = FixtureContribution(&cfg, 2U, 250U, b, 2U);
    UmiAiTrainingFederatedContribution cc = FixtureContribution(&cfg, 3U, 150U, c, 2U);
    CHECK(UmiAiTrainingFederatedRoundSubmit(round, &ca) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingFederatedRoundSubmit(round, &cb) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingFederatedRoundSubmit(round, &cc) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingFederatedRoundFinalise(round, mean, 2U, &report) == UMI_STATUS_OK);
    CHECK(fabsf(mean[0] - 0.029F) < 0.000001F);
    CHECK(fabsf(mean[1] - 0.050F) < 0.000001F);
    CHECK(report.totalDistinctSamples == 500U && report.acceptedContributors == 3U);
    CHECK(report.finalised == 1 && report.parameterCount == 2U);
    CHECK(UmiAiTrainingFederatedRoundFinalise(round, mean, 2U, NULL) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingFederatedRoundSubmit(round, &ca) == UMI_STATUS_INVALID_STATE);
    UmiAiTrainingFederatedRoundDestroy(round);
    puts("Three-worker weighted FedAvg arithmetic passed");
    return 0;
}
