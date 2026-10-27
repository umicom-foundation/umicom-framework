/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_federated_round_rejected.c
 *
 * PURPOSE:
 *   Test duplicate/stale/oversized/non-finite worker rejection without
 *   partial aggregation publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "federated_round_fixture.h"
#include <stdint.h>
int main(void)
{
    UmiAiTrainingFederatedRoundConfig cfg = FixtureConfig(2U, 2U);
    UmiAiTrainingFederatedRound *round = NULL;
    const float first[2] = {0.10F, 0.25F};
    const float wrong[2] = {20.0F, 0.0F};
    const float nanValue[2] = {NAN, 0.0F};
    float result[2] = {123.0F, 234.0F};
    UmiAiTrainingFederatedRoundReport info = {0};
    CHECK(UmiAiTrainingFederatedRoundCreate(&cfg, &round) == UMI_STATUS_OK);
    UmiAiTrainingFederatedContribution ca = FixtureContribution(&cfg, 1U, 3U, first, 2U);
    UmiAiTrainingFederatedContribution cb = FixtureContribution(&cfg, 2U, 5U, wrong, 2U);
    CHECK(UmiAiTrainingFederatedRoundFinalise(round, result, 2U, NULL) == UMI_STATUS_INVALID_STATE);
    CHECK(result[0] == 123.0F && result[1] == 234.0F);
    CHECK(UmiAiTrainingFederatedRoundSubmit(round, &ca) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingFederatedRoundSubmit(round, &ca) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(UmiAiTrainingFederatedRoundSubmit(round, &cb) == UMI_STATUS_INVALID_ARGUMENT);
    cb.weightSpaceDelta = nanValue;
    CHECK(UmiAiTrainingFederatedRoundSubmit(round, &cb) == UMI_STATUS_INVALID_ARGUMENT);
    cb.weightSpaceDelta = first;
    cb.baseCheckpointSha256[0] = 12U;
    CHECK(UmiAiTrainingFederatedRoundSubmit(round, &cb) == UMI_STATUS_INVALID_STATE);
    cb.baseCheckpointSha256[0] = cfg.baseCheckpointSha256[0];
    cb.distinctSampleCount = UINT64_MAX;
    CHECK(UmiAiTrainingFederatedRoundSubmit(round, &cb) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(UmiAiTrainingFederatedRoundInspect(round, &info) == UMI_STATUS_OK);
    CHECK(info.acceptedContributors == 1U && info.totalDistinctSamples == 3U);
    CHECK(UmiAiTrainingFederatedRoundFinalise(round, result, 1U, NULL) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(result[0] == 123.0F && result[1] == 234.0F);
    CHECK(UmiAiTrainingFederatedRoundFinalise(round, result, 2U, NULL) == UMI_STATUS_OK);
    CHECK(fabsf(result[0] - first[0]) < 0.000001F);
    UmiAiTrainingFederatedRoundDestroy(round);
    puts("Rejected contributions and short outputs remain failure-atomic");
    return 0;
}
