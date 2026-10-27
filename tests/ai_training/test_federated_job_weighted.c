/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_federated_job_weighted.c
 *
 * PURPOSE:
 *   Prove that three admitted updates reach the real Framework FedAvg round,
 *   with sample-weighted results and no pending reservations on finalisation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "federated_job_fixture.h"

int main(void)
{
    UmiAiTrainingFederatedJobConfig cfg = TestJobConfig();
    UmiAiTrainingFederatedJob *job = NULL;
    UmiAiTrainingFederatedTicket t[3] = {{0}};
    const uint64_t counts[3] = {100U, 250U, 150U};
    const float delta[3][2] = {{0.05F, 0.2F}, {-0.01F, 0.4F}, {0.08F, -0.1F}};
    UmiAiTrainingFederatedJobReport state = {0};
    UmiAiTrainingFederatedRoundReport agg = {0};
    float result[2] = {999.0F, 999.0F};
    VERIFY(UmiAiTrainingFederatedJobCreate(&cfg, &job) == UMI_STATUS_OK);
    for (size_t i = 0U; i < 3U; ++i) {
        const UmiAiTrainingFederatedAssignment a = TestAssignment(
            &cfg, (uint8_t)(i + 1U), counts[i], 20U);
        UmiAiTrainingFederatedContribution c = TestContribution(&a, delta[i]);
        VERIFY(UmiAiTrainingFederatedJobAdmit(job, &a, &t[i]) == UMI_STATUS_OK);
        VERIFY(UmiAiTrainingFederatedJobComplete(job, t[i], &c) == UMI_STATUS_OK);
    }
    VERIFY(UmiAiTrainingFederatedJobFinalise(job, result, 2U, &agg) == UMI_STATUS_OK);
    VERIFY(fabsf(result[0] - 0.029F) < 0.000001F);
    /* (100*.2 + 250*.4 + 150*(-.1))/500 = .21 */
    VERIFY(fabsf(result[1] - 0.21F) < 0.000001F);
    VERIFY(agg.acceptedContributors == 3U && agg.totalDistinctSamples == 500U);
    VERIFY(UmiAiTrainingFederatedJobInspect(job, &state) == UMI_STATUS_OK);
    VERIFY(state.completedAssignments == 3U && state.pendingAssignments == 0U);
    VERIFY(state.reservedTransferBytes == 0U && state.peakReservedTransferBytes == 20U);
    VERIFY(state.finalised == 1 && state.acceptedDistinctSamples == 500U);
    VERIFY(UmiAiTrainingFederatedJobComplete(job, t[0],
        &(UmiAiTrainingFederatedContribution){0}) == UMI_STATUS_INVALID_STATE);
    UmiAiTrainingFederatedJobDestroy(job);
    puts("Federated-job weighted round: real C23 arithmetic, 3/3 admitted");
    return 0;
}
