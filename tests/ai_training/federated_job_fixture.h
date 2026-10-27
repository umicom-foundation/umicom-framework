/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/federated_job_fixture.h
 *
 * PURPOSE:
 *   Share deterministic real-aggregator fixtures for the native C23
 *   federated assignment, cancellation and identity regression tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FEDERATED_JOB_FIXTURE_H
#define UMICOM_FEDERATED_JOB_FIXTURE_H

#include "umicom/ai/training/federated_job.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define VERIFY(condition) do { if (!(condition)) { \
    fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #condition); \
    return 1; \
} } while (0)

static inline UmiAiTrainingFederatedJobConfig TestJobConfig(void)
{
    UmiAiTrainingFederatedJobConfig c = {0};
    c.round.parameterCount = 2U;
    c.round.maximumContributors = 5U;
    c.round.maximumDeltaL2Norm = 10.0F;
    memset(c.round.baseCheckpointSha256, 0x51,
           sizeof(c.round.baseCheckpointSha256));
    memset(c.dataPolicySha256, 0x61, sizeof(c.dataPolicySha256));
    c.roundId = UINT64_C(737);
    c.maximumLocalSteps = UINT64_C(250);
    c.maximumSamplesPerAssignment = UINT64_C(500);
    c.maximumTransferBytesPerAssignment = 100U;
    c.maximumReservedTransferBytes = 100U;
    return c;
}

static inline UmiAiTrainingFederatedAssignment TestAssignment(
    const UmiAiTrainingFederatedJobConfig *c,
    uint8_t id, uint64_t samples, size_t reserved)
{
    UmiAiTrainingFederatedAssignment a = {0};
    memset(a.contributorId, id, sizeof(a.contributorId));
    memset(a.datasetCommitmentSha256, (int)(id + 25U),
           sizeof(a.datasetCommitmentSha256));
    memcpy(a.baseCheckpointSha256, c->round.baseCheckpointSha256,
           sizeof(a.baseCheckpointSha256));
    memcpy(a.dataPolicySha256, c->dataPolicySha256,
           sizeof(a.dataPolicySha256));
    a.distinctSampleCount = samples;
    a.localSteps = 12U;
    a.declaredTransferBytes = reserved;
    return a;
}

static inline UmiAiTrainingFederatedContribution TestContribution(
    const UmiAiTrainingFederatedAssignment *a, const float delta[2])
{
    UmiAiTrainingFederatedContribution c = {0};
    memcpy(c.contributorId, a->contributorId, sizeof(c.contributorId));
    memcpy(c.baseCheckpointSha256, a->baseCheckpointSha256,
           sizeof(c.baseCheckpointSha256));
    memcpy(c.datasetCommitmentSha256, a->datasetCommitmentSha256,
           sizeof(c.datasetCommitmentSha256));
    c.distinctSampleCount = a->distinctSampleCount;
    c.weightSpaceDelta = delta;
    c.deltaCount = 2U;
    return c;
}
#endif
