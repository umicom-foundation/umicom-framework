/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: learning/native-ai-federated-job/demo.c
 *
 * PURPOSE:
 *   Demonstrate local three-worker admission, cancellation-free completion
 *   and the existing native FedAvg result without claiming network training.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/federated_job.h"
#include <stdio.h>
#include <string.h>

int main(void)
{
    UmiAiTrainingFederatedJobConfig cfg = {0};
    UmiAiTrainingFederatedJob *job = NULL;
    const float values[3][1] = {{0.05F}, {-0.01F}, {0.08F}};
    const uint64_t counts[3] = {100U, 250U, 150U};
    float output = 0.0F;
    UmiAiTrainingFederatedRoundReport report = {0};
    cfg.round.parameterCount = 1U;
    cfg.round.maximumContributors = 3U;
    cfg.round.maximumDeltaL2Norm = 1.0F;
    memset(cfg.round.baseCheckpointSha256, 0x33, 32U);
    memset(cfg.dataPolicySha256, 0x44, 32U);
    cfg.roundId = 42U;
    cfg.maximumLocalSteps = 40U;
    cfg.maximumSamplesPerAssignment = 500U;
    cfg.maximumTransferBytesPerAssignment = 256U;
    cfg.maximumReservedTransferBytes = 512U;
    if (UmiAiTrainingFederatedJobCreate(&cfg, &job) != UMI_STATUS_OK) return 1;
    for (size_t i = 0U; i < 3U; ++i) {
        UmiAiTrainingFederatedAssignment a = {0};
        UmiAiTrainingFederatedTicket ticket = {0};
        UmiAiTrainingFederatedContribution contribution = {0};
        memset(a.contributorId, (int)(i+1U), 32U);
        memset(a.datasetCommitmentSha256, (int)(i+21U), 32U);
        memcpy(a.baseCheckpointSha256, cfg.round.baseCheckpointSha256, 32U);
        memcpy(a.dataPolicySha256, cfg.dataPolicySha256, 32U);
        a.distinctSampleCount = counts[i];
        a.localSteps = 12U;
        a.declaredTransferBytes = 16U;
        if (UmiAiTrainingFederatedJobAdmit(job, &a, &ticket) != UMI_STATUS_OK)
            goto fail;
        memcpy(contribution.contributorId, a.contributorId, 32U);
        memcpy(contribution.datasetCommitmentSha256, a.datasetCommitmentSha256, 32U);
        memcpy(contribution.baseCheckpointSha256, a.baseCheckpointSha256, 32U);
        contribution.distinctSampleCount = a.distinctSampleCount;
        contribution.weightSpaceDelta = values[i];
        contribution.deltaCount = 1U;
        if (UmiAiTrainingFederatedJobComplete(job, ticket, &contribution) != UMI_STATUS_OK)
            goto fail;
    }
    if (UmiAiTrainingFederatedJobFinalise(job, &output, 1U, &report) != UMI_STATUS_OK)
        goto fail;
    printf("Three local, synthetic workers: weighted delta = %.6f (samples=%llu)\n",
           (double)output, (unsigned long long)report.totalDistinctSamples);
    puts("No real training, transport, signatures, privacy or blockchain are claimed.");
    UmiAiTrainingFederatedJobDestroy(job);
    return 0;
fail:
    UmiAiTrainingFederatedJobDestroy(job);
    return 1;
}
