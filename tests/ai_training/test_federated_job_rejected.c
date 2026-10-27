/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_federated_job_rejected.c
 *
 * PURPOSE:
 *   Reject mismatched data policies, base checkpoints, declared samples,
 *   malformed vectors and excessive norms without advancing the numeric round.
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
    UmiAiTrainingFederatedTicket untouched = {99U, 88U, 77U}, ticket = {0};
    UmiAiTrainingFederatedAssignment a = TestAssignment(&cfg, 4U, 50U, 10U);
    UmiAiTrainingFederatedJobReport state = {0};
    const float valid[2] = {0.4F, 0.5F};
    const float invalid[2] = {NAN, 0.5F};
    VERIFY(UmiAiTrainingFederatedJobCreate(&cfg, &job) == UMI_STATUS_OK);
    a.dataPolicySha256[0] ^= 0x55U;
    VERIFY(UmiAiTrainingFederatedJobAdmit(job, &a, &untouched) ==
           UMI_STATUS_INVALID_ARGUMENT);
    VERIFY(untouched.jobIdentity == 99U && untouched.serial == 88U);
    a.dataPolicySha256[0] ^= 0x55U;
    a.baseCheckpointSha256[0] ^= 0x01U;
    VERIFY(UmiAiTrainingFederatedJobAdmit(job, &a, &untouched) ==
           UMI_STATUS_INVALID_ARGUMENT);
    a.baseCheckpointSha256[0] ^= 0x01U;
    a.localSteps = 251U;
    VERIFY(UmiAiTrainingFederatedJobAdmit(job, &a, &untouched) ==
           UMI_STATUS_INVALID_ARGUMENT);
    a.localSteps = 12U;
    VERIFY(UmiAiTrainingFederatedJobAdmit(job, &a, &ticket) == UMI_STATUS_OK);
    UmiAiTrainingFederatedContribution c = TestContribution(&a, valid);
    c.distinctSampleCount = 51U;
    VERIFY(UmiAiTrainingFederatedJobComplete(job, ticket, &c) ==
           UMI_STATUS_INVALID_STATE);
    c.distinctSampleCount = 50U;
    c.datasetCommitmentSha256[1] ^= 1U;
    VERIFY(UmiAiTrainingFederatedJobComplete(job, ticket, &c) ==
           UMI_STATUS_INVALID_STATE);
    c.datasetCommitmentSha256[1] ^= 1U;
    c.weightSpaceDelta = invalid;
    VERIFY(UmiAiTrainingFederatedJobComplete(job, ticket, &c) ==
           UMI_STATUS_INVALID_ARGUMENT);
    VERIFY(UmiAiTrainingFederatedJobInspect(job, &state) == UMI_STATUS_OK);
    VERIFY(state.pendingAssignments == 1U && state.completedAssignments == 0U);
    VERIFY(state.reservedTransferBytes == 10U && state.acceptedDistinctSamples == 0U);
    c.weightSpaceDelta = valid;
    VERIFY(UmiAiTrainingFederatedJobComplete(job, ticket, &c) == UMI_STATUS_OK);
    float result[2] = {0};
    VERIFY(UmiAiTrainingFederatedJobFinalise(job, result, 2U, NULL) == UMI_STATUS_OK);
    VERIFY(fabsf(result[1]-0.5F) < 0.000001F);
    UmiAiTrainingFederatedJobDestroy(job);
    puts("Federated-job rejections are failure-atomic across the real round");
    return 0;
}
