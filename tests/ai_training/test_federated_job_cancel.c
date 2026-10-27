/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_federated_job_cancel.c
 *
 * PURPOSE:
 *   Confirm declared-byte admission, cancellation, stale tickets and refusal
 *   to finalise a round with outstanding worker assignments.
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
    UmiAiTrainingFederatedTicket old = {0}, fresh = {0}, unchanged = {55U, 55U, 55U};
    UmiAiTrainingFederatedJobReport state = {0};
    const float delta[2] = {0.25F, 0.50F};
    float result[2] = {3.0F, 3.0F};
    cfg.maximumReservedTransferBytes = 30U;
    const UmiAiTrainingFederatedAssignment a = TestAssignment(&cfg, 1U, 30U, 24U);
    const UmiAiTrainingFederatedAssignment b = TestAssignment(&cfg, 2U, 50U, 24U);
    VERIFY(UmiAiTrainingFederatedJobCreate(&cfg, &job) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingFederatedJobAdmit(job, &a, &old) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingFederatedJobAdmit(job, &b, &unchanged) ==
           UMI_STATUS_CAPACITY_EXCEEDED);
    VERIFY(unchanged.jobIdentity == 55U && unchanged.serial == 55U);
    VERIFY(UmiAiTrainingFederatedJobFinalise(job, result, 2U, NULL) ==
           UMI_STATUS_INVALID_STATE);
    VERIFY(result[0] == 3.0F && result[1] == 3.0F);
    VERIFY(UmiAiTrainingFederatedJobCancel(job, old) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingFederatedJobCancel(job, old) == UMI_STATUS_INVALID_STATE);
    VERIFY(UmiAiTrainingFederatedJobAdmit(job, &a, &unchanged) ==
           UMI_STATUS_ALREADY_EXISTS);
    VERIFY(UmiAiTrainingFederatedJobFinalise(job, result, 2U, NULL) ==
           UMI_STATUS_INVALID_STATE); /* zero completed deltas */
    VERIFY(UmiAiTrainingFederatedJobAdmit(job, &b, &fresh) == UMI_STATUS_OK);
    UmiAiTrainingFederatedContribution contribution = TestContribution(&b, delta);
    VERIFY(UmiAiTrainingFederatedJobComplete(job, fresh, &contribution) ==
           UMI_STATUS_OK);
    VERIFY(UmiAiTrainingFederatedJobFinalise(job, result, 2U, NULL) ==
           UMI_STATUS_OK);
    VERIFY(fabsf(result[0]-0.25F) < 0.000001F);
    VERIFY(UmiAiTrainingFederatedJobInspect(job, &state) == UMI_STATUS_OK);
    VERIFY(state.cancelledAssignments == 1U && state.completedAssignments == 1U);
    VERIFY(state.reservedTransferBytes == 0U && state.peakReservedTransferBytes == 24U);
    UmiAiTrainingFederatedJobDestroy(job);
    puts("Federated-job cancellation: budget and stale ticket verified");
    return 0;
}
