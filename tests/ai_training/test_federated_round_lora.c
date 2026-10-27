/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_federated_round_lora.c
 *
 * PURPOSE:
 *   Demonstrate why federated LoRA must aggregate weight-space deltas
 *   rather than factor-wise averages.
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
    UmiAiTrainingFederatedRoundConfig cfg = FixtureConfig(1U, 2U);
    UmiAiTrainingFederatedRound *round = NULL;
    /* Worker one: B=+1, A=+1; worker two: B=-1, A=-1.
     * Both imply the same scalar weight-space delta of +1. Averaging
     * the factors separately would incorrectly produce (0)*(0)=0. */
    const float firstDelta[1] = {1.0F};
    const float secondDelta[1] = {1.0F};
    float output[1] = {-6.0F};
    CHECK(UmiAiTrainingFederatedRoundCreate(&cfg, &round) == UMI_STATUS_OK);
    UmiAiTrainingFederatedContribution ca = FixtureContribution(&cfg, 3U, 1U, firstDelta, 1U);
    UmiAiTrainingFederatedContribution cb = FixtureContribution(&cfg, 4U, 1U, secondDelta, 1U);
    CHECK(UmiAiTrainingFederatedRoundSubmit(round, &ca) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingFederatedRoundSubmit(round, &cb) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingFederatedRoundFinalise(round, output, 1U, NULL) == UMI_STATUS_OK);
    CHECK(output[0] == 1.0F);
    UmiAiTrainingFederatedRoundDestroy(round);
    puts("True delta aggregation rejects the naive factor-averaging trap");
    return 0;
}
