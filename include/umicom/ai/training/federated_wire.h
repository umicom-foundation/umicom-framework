/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/federated_wire.h
 *
 * PURPOSE:
 *   Define a bounded, canonical, platform-independent binary representation
 *   for externally authorised federated assignments and returned FP32 deltas.
 *   Compose existing Framework SHA-256, job admission and message contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_FEDERATED_WIRE_H
#define UMICOM_AI_TRAINING_FEDERATED_WIRE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/federated_job.h"
#include "umicom/messaging/message.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_FEDERATED_WIRE_HEADER_BYTES 184U
#define UMI_AI_FEDERATED_WIRE_CHECKSUM_BYTES 32U
#define UMI_AI_FEDERATED_WIRE_MAX_BYTES \
    (UMI_AI_FEDERATED_WIRE_HEADER_BYTES + \
     4U * UMI_AI_FEDERATED_MAX_PARAMETERS + \
     UMI_AI_FEDERATED_WIRE_CHECKSUM_BYTES)

typedef enum UmiAiTrainingFederatedWireKind {
    UMI_AI_FEDERATED_WIRE_ASSIGNMENT = 1,
    UMI_AI_FEDERATED_WIRE_RESULT = 2
} UmiAiTrainingFederatedWireKind;

typedef struct UmiAiTrainingFederatedWireRecord {
    UmiAiTrainingFederatedWireKind kind;
    uint64_t roundId;
    uint64_t sequence; /* Public round-scoped correlation, NOT a job ticket. */
    UmiAiTrainingFederatedAssignment assignment;
    /* RESULT only: caller-owned weight-space delta; never LoRA factors. */
    const float *delta;
    size_t deltaCount;
} UmiAiTrainingFederatedWireRecord;

/* Schema 1, big-endian integer/IEEE-754 binary32 encoding:
 * magic[4], version[2], kind[2], exact frameLength[4], round[8],
 * sequence[8], contributor[32], base[32], shard[32], policy[32],
 * distinctSamples[8], localSteps[8], declaredTransferBytes[8],
 * deltaCount[4], deltaBits[4 * count], sha256[32].
 * The SHA-256 digest uses Framework's existing streaming implementation and a
 * fixed domain prefix. It detects byte changes, not sender authenticity,
 * training honesty, confidentiality, replay or permission to access data.
 * Never transmit a UmiAiTrainingFederatedTicket: tickets are process-local.
 * An external service must authenticate/authorise and match round+sequence
 * to its own stored pending ticket before calling FederatedJobComplete.
 * A RESULT must contain 1..4096 finite delta floats; an ASSIGNMENT has none.
 * Failure leaves all caller-visible outputs unchanged. No heap allocations.
 * All buffers and messages are caller-owned and caller-serialised. */
UmiStatus UmiAiTrainingFederatedWireMeasure(
    const UmiAiTrainingFederatedWireRecord *record,
    size_t *outBytes);
UmiStatus UmiAiTrainingFederatedWireEncode(
    const UmiAiTrainingFederatedWireRecord *record,
    uint8_t *outFrame, size_t frameCapacity, size_t *outFrameBytes);
UmiStatus UmiAiTrainingFederatedWireDecode(
    const uint8_t *frame, size_t frameBytes,
    UmiAiTrainingFederatedWireRecord *outRecord,
    float *outDelta, size_t deltaCapacity);

/* Match externally authenticated assignment to a previously admitted ticket.
 * This checks identity fields only, not a signature or authorisation decision. */
UmiStatus UmiAiTrainingFederatedWireMatchResult(
    const UmiAiTrainingFederatedWireRecord *result,
    const UmiAiTrainingFederatedAssignment *admitted,
    uint64_t roundId, uint64_t sequence);

/* Construct a borrowed canonical numeric contribution for the existing
 * Framework-owned federated-job/round services. Caller must preserve the
 * decoded delta buffer until Complete returns. */
UmiStatus UmiAiTrainingFederatedWireContribution(
    const UmiAiTrainingFederatedWireRecord *result,
    UmiAiTrainingFederatedContribution *outContribution);

/* Wrap an already verified frame with the EXISTING Framework typed envelope.
 * Payload is borrowed; call umi_message_copy for queue/persistence ownership.
 * This is not a transport or authentication implementation. */
UmiStatus UmiAiTrainingFederatedWireBindEnvelope(
    const uint8_t *frame, size_t frameBytes,
    UmiMessageEnvelope *outEnvelope);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_FEDERATED_WIRE_H */
