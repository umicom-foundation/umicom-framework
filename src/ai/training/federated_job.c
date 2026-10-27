/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/federated_job.c
 *
 * PURPOSE:
 *   Enforce immutable, bounded federated-job assignments and failure-atomic
 *   completion/cancellation while reusing the canonical Framework
 *   UmiAiTrainingFederatedRound for every numeric model-delta calculation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/federated_job.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

enum { UMI_JOB_EMPTY = 0, UMI_JOB_PENDING = 1,
       UMI_JOB_COMPLETE = 2, UMI_JOB_CANCELLED = 3 };

typedef struct UmiJobEntry {
    UmiAiTrainingFederatedAssignment assignment;
    uint64_t serial;
    int state;
} UmiJobEntry;

struct UmiAiTrainingFederatedJob {
    UmiAiTrainingFederatedJobConfig config;
    UmiAiTrainingFederatedRound *round;
    uint64_t identity;
    uint64_t nextSerial;
    size_t assigned;
    size_t pending;
    size_t completed;
    size_t cancelled;
    size_t reserved;
    size_t peakReserved;
    int finalised;
    UmiJobEntry entries[UMI_AI_FEDERATED_MAX_CONTRIBUTORS];
};

/* Job objects are caller-serialised. This counter makes in-process stale
 * tickets from unrelated jobs distinct; it is NOT an interprocess ID. A
 * reserved identity is not reused, even if creation later fails. */
static uint64_t nextProcessLocalIdentity = UINT64_C(1);

static int DigestNonzero(const uint8_t digest[UMI_AI_FEDERATED_DIGEST_BYTES])
{
    unsigned int present = 0U;
    for (size_t i = 0U; i < UMI_AI_FEDERATED_DIGEST_BYTES; ++i)
        present |= (unsigned int)digest[i];
    return present != 0U;
}

static int DigestEquals(const uint8_t a[UMI_AI_FEDERATED_DIGEST_BYTES],
                        const uint8_t b[UMI_AI_FEDERATED_DIGEST_BYTES])
{
    return memcmp(a, b, UMI_AI_FEDERATED_DIGEST_BYTES) == 0;
}

static UmiJobEntry *PendingEntry(UmiAiTrainingFederatedJob *job,
                                  UmiAiTrainingFederatedTicket ticket)
{
    if (job == NULL || ticket.jobIdentity != job->identity ||
        ticket.slotIndex >= job->assigned || ticket.serial == UINT64_C(0))
        return NULL;
    UmiJobEntry *entry = &job->entries[ticket.slotIndex];
    if (entry->serial != ticket.serial || entry->state != UMI_JOB_PENDING)
        return NULL;
    return entry;
}

UmiStatus UmiAiTrainingFederatedJobCreate(
    const UmiAiTrainingFederatedJobConfig *config,
    UmiAiTrainingFederatedJob **outJob)
{
    if (config == NULL || outJob == NULL || *outJob != NULL ||
        config->roundId == UINT64_C(0) ||
        config->maximumLocalSteps == UINT64_C(0) ||
        config->maximumSamplesPerAssignment == UINT64_C(0) ||
        config->maximumTransferBytesPerAssignment == 0U ||
        !DigestNonzero(config->dataPolicySha256) ||
        nextProcessLocalIdentity == UINT64_MAX)
        return UMI_STATUS_INVALID_ARGUMENT;

    UmiAiTrainingFederatedJob *created = calloc(1U, sizeof(*created));
    if (created == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UmiAiTrainingFederatedRoundCreate(
        &config->round, &created->round);
    if (status != UMI_STATUS_OK) {
        free(created);
        return status;
    }
    created->config = *config;
    created->identity = nextProcessLocalIdentity++;
    created->nextSerial = UINT64_C(1);
    *outJob = created;
    return UMI_STATUS_OK;
}

void UmiAiTrainingFederatedJobDestroy(UmiAiTrainingFederatedJob *job)
{
    if (job == NULL) return;
    UmiAiTrainingFederatedRoundDestroy(job->round);
    /* Best effort hygiene; standard memset is not a secure-erasure promise. */
    memset(job->entries, 0, sizeof(job->entries));
    free(job);
}

UmiStatus UmiAiTrainingFederatedJobAdmit(
    UmiAiTrainingFederatedJob *job,
    const UmiAiTrainingFederatedAssignment *assignment,
    UmiAiTrainingFederatedTicket *outTicket)
{
    if (job == NULL || assignment == NULL || outTicket == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (job->finalised) return UMI_STATUS_INVALID_STATE;
    if (job->assigned >= job->config.round.maximumContributors ||
        job->nextSerial == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (!DigestNonzero(assignment->contributorId) ||
        !DigestNonzero(assignment->datasetCommitmentSha256) ||
        !DigestEquals(assignment->baseCheckpointSha256,
                      job->config.round.baseCheckpointSha256) ||
        !DigestEquals(assignment->dataPolicySha256,
                      job->config.dataPolicySha256))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (assignment->localSteps == UINT64_C(0) ||
        assignment->localSteps > job->config.maximumLocalSteps ||
        assignment->distinctSampleCount == UINT64_C(0) ||
        assignment->distinctSampleCount >
            job->config.maximumSamplesPerAssignment ||
        assignment->declaredTransferBytes == 0U ||
        assignment->declaredTransferBytes >
            job->config.maximumTransferBytesPerAssignment)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < job->assigned; ++i) {
        if (DigestEquals(job->entries[i].assignment.contributorId,
                         assignment->contributorId))
            return UMI_STATUS_ALREADY_EXISTS;
    }
    if (assignment->declaredTransferBytes > SIZE_MAX - job->reserved)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    const size_t newReserved = job->reserved + assignment->declaredTransferBytes;
    if (job->config.maximumReservedTransferBytes != 0U &&
        newReserved > job->config.maximumReservedTransferBytes)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    const size_t index = job->assigned;
    const uint64_t serial = job->nextSerial;
    /* No mutation after successful validation can fail. */
    job->entries[index].assignment = *assignment;
    job->entries[index].serial = serial;
    job->entries[index].state = UMI_JOB_PENDING;
    ++job->assigned;
    ++job->pending;
    job->reserved = newReserved;
    if (newReserved > job->peakReserved) job->peakReserved = newReserved;
    ++job->nextSerial;
    const UmiAiTrainingFederatedTicket result = {
        job->identity, serial, index
    };
    *outTicket = result;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingFederatedJobComplete(
    UmiAiTrainingFederatedJob *job,
    UmiAiTrainingFederatedTicket ticket,
    const UmiAiTrainingFederatedContribution *contribution)
{
    if (job == NULL || contribution == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (job->finalised) return UMI_STATUS_INVALID_STATE;
    UmiJobEntry *entry = PendingEntry(job, ticket);
    if (entry == NULL) return UMI_STATUS_INVALID_STATE;
    const UmiAiTrainingFederatedAssignment *a = &entry->assignment;
    if (!DigestEquals(contribution->contributorId, a->contributorId) ||
        !DigestEquals(contribution->baseCheckpointSha256,
                      a->baseCheckpointSha256) ||
        !DigestEquals(contribution->datasetCommitmentSha256,
                      a->datasetCommitmentSha256) ||
        contribution->distinctSampleCount != a->distinctSampleCount)
        return UMI_STATUS_INVALID_STATE;

    /* Reuse the established Framework numeric authority: failed submissions
     * leave both the round and the pending admission unchanged. */
    UmiStatus status = UmiAiTrainingFederatedRoundSubmit(job->round, contribution);
    if (status != UMI_STATUS_OK) return status;
    entry->state = UMI_JOB_COMPLETE;
    --job->pending;
    ++job->completed;
    job->reserved -= a->declaredTransferBytes;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingFederatedJobCancel(
    UmiAiTrainingFederatedJob *job,
    UmiAiTrainingFederatedTicket ticket)
{
    UmiJobEntry *entry = PendingEntry(job, ticket);
    if (entry == NULL || job->finalised) return UMI_STATUS_INVALID_STATE;
    entry->state = UMI_JOB_CANCELLED;
    --job->pending;
    ++job->cancelled;
    job->reserved -= entry->assignment.declaredTransferBytes;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingFederatedJobFinalise(
    UmiAiTrainingFederatedJob *job,
    float *outMeanDelta, size_t capacity,
    UmiAiTrainingFederatedRoundReport *outRoundReport)
{
    if (job == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (job->pending != 0U) return UMI_STATUS_INVALID_STATE;
    UmiStatus status = UmiAiTrainingFederatedRoundFinalise(
        job->round, outMeanDelta, capacity, outRoundReport);
    if (status == UMI_STATUS_OK) job->finalised = 1;
    return status;
}

UmiStatus UmiAiTrainingFederatedJobInspect(
    const UmiAiTrainingFederatedJob *job,
    UmiAiTrainingFederatedJobReport *outReport)
{
    if (job == NULL || outReport == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiAiTrainingFederatedRoundReport round = {0};
    UmiStatus status = UmiAiTrainingFederatedRoundInspect(job->round, &round);
    if (status != UMI_STATUS_OK) return status;
    UmiAiTrainingFederatedJobReport result = {0};
    result.roundId = job->config.roundId;
    result.assignedTotal = job->assigned;
    result.pendingAssignments = job->pending;
    result.completedAssignments = job->completed;
    result.cancelledAssignments = job->cancelled;
    result.reservedTransferBytes = job->reserved;
    result.peakReservedTransferBytes = job->peakReserved;
    result.acceptedDistinctSamples = round.totalDistinctSamples;
    result.finalised = job->finalised;
    *outReport = result;
    return UMI_STATUS_OK;
}
