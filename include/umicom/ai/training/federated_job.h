/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/federated_job.h
 *
 * PURPOSE:
 *   Coordinate a bounded, round-scoped local federated-learning experiment:
 *   admit independently authorised assignments, account for claimed transfer
 *   budgets, cancel pending assignments and submit accepted weight-space
 *   deltas through the existing Framework federated-round arithmetic owner.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_FEDERATED_JOB_H
#define UMICOM_AI_TRAINING_FEDERATED_JOB_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/federated_round.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiTrainingFederatedJob UmiAiTrainingFederatedJob;

/* A job lasts for one local, serialised aggregation round. The caller MUST
 * independently authorise worker IDs, shard access and base-checkpoint
 * identity before submitting an assignment; nonzero digests do not do that.
 * No networking, model training, signature validation, or ledger is provided.
 * A zero maximumReservedTransferBytes denotes unlimited declared transfer
 * reservations; all other maxima must be positive. */
typedef struct UmiAiTrainingFederatedJobConfig {
    UmiAiTrainingFederatedRoundConfig round;
    uint64_t roundId;
    uint64_t maximumLocalSteps;
    uint64_t maximumSamplesPerAssignment;
    size_t maximumTransferBytesPerAssignment;
    size_t maximumReservedTransferBytes;
    uint8_t dataPolicySha256[UMI_AI_FEDERATED_DIGEST_BYTES];
} UmiAiTrainingFederatedJobConfig;

/* Byte reservations are forecasts, NOT actual measured network traffic. The
 * payload size may reflect a proposed compression contract, even though the
 * existing aggregator always consumes a full FP32 weight-space vector. */
typedef struct UmiAiTrainingFederatedAssignment {
    uint8_t contributorId[UMI_AI_FEDERATED_DIGEST_BYTES];
    uint8_t baseCheckpointSha256[UMI_AI_FEDERATED_DIGEST_BYTES];
    uint8_t datasetCommitmentSha256[UMI_AI_FEDERATED_DIGEST_BYTES];
    uint8_t dataPolicySha256[UMI_AI_FEDERATED_DIGEST_BYTES];
    uint64_t distinctSampleCount;
    uint64_t localSteps;
    size_t declaredTransferBytes;
} UmiAiTrainingFederatedAssignment;

/* A ticket is a process-local generation-stamped capability, NOT a network
 * credential or cryptographic proof. It becomes invalid after acceptance,
 * cancellation or destruction of the owning job. */
typedef struct UmiAiTrainingFederatedTicket {
    uint64_t jobIdentity;
    uint64_t serial;
    size_t slotIndex;
} UmiAiTrainingFederatedTicket;

typedef struct UmiAiTrainingFederatedJobReport {
    uint64_t roundId;
    size_t assignedTotal;
    size_t pendingAssignments;
    size_t completedAssignments;
    size_t cancelledAssignments;
    size_t reservedTransferBytes;
    size_t peakReservedTransferBytes;
    uint64_t acceptedDistinctSamples;
    int finalised;
} UmiAiTrainingFederatedJobReport;

/* Initialise *outJob to NULL. A failed request leaves it untouched. Internally
 * reuse UmiAiTrainingFederatedRoundCreate; this does not copy its maths. */
UmiStatus UmiAiTrainingFederatedJobCreate(
    const UmiAiTrainingFederatedJobConfig *config,
    UmiAiTrainingFederatedJob **outJob);
void UmiAiTrainingFederatedJobDestroy(UmiAiTrainingFederatedJob *job);

/* Admit an externally authorised assignment. Reject duplicate worker IDs
 * even after cancellation, to prohibit re-admission under the same identity
 * within one round. Assigned slots are never recycled within one round.
 * Only a successful admission publishes outTicket. */
UmiStatus UmiAiTrainingFederatedJobAdmit(
    UmiAiTrainingFederatedJob *job,
    const UmiAiTrainingFederatedAssignment *assignment,
    UmiAiTrainingFederatedTicket *outTicket);

/* Accept one complete numeric contribution through the established round.
 * The submitted contributor/shard/base IDs and declared sample count MUST
 * equal the immutable admission metadata. The aggregator checks the actual
 * FP32 vector, its norm and duplicate identity. On rejection neither the
 * admission state nor the underlying numeric round advances. A successful
 * submission closes the ticket and frees the *reservation* only. */
UmiStatus UmiAiTrainingFederatedJobComplete(
    UmiAiTrainingFederatedJob *job,
    UmiAiTrainingFederatedTicket ticket,
    const UmiAiTrainingFederatedContribution *contribution);

/* A pending job may be cancelled once, without recording a numeric update. */
UmiStatus UmiAiTrainingFederatedJobCancel(
    UmiAiTrainingFederatedJob *job,
    UmiAiTrainingFederatedTicket ticket);

/* No outstanding assignments may remain at finalisation. At least one real
 * contribution is required. All mean-delta mathematics and output-overlap
 * checks are delegated to UmiAiTrainingFederatedRoundFinalise.
 * This does NOT update a live model or certify statistical usefulness. */
UmiStatus UmiAiTrainingFederatedJobFinalise(
    UmiAiTrainingFederatedJob *job,
    float *outMeanDelta, size_t capacity,
    UmiAiTrainingFederatedRoundReport *outRoundReport);

UmiStatus UmiAiTrainingFederatedJobInspect(
    const UmiAiTrainingFederatedJob *job,
    UmiAiTrainingFederatedJobReport *outReport);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_FEDERATED_JOB_H */
