/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/federated_wire.c
 *
 * PURPOSE:
 *   Encode and validate exact-size, portable federated-learning wire frames
 *   while reusing the Framework SHA-256, job and typed message contracts.
 *   Reject malformed, non-finite and truncated messages without publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/federated_wire.h"
#include "umicom/base/sha256.h"

#include <float.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

_Static_assert(sizeof(float) == 4U && FLT_RADIX == 2 && FLT_MANT_DIG == 24 &&
               FLT_MAX_EXP == 128, "Wire format requires IEEE-754 binary32");

static const uint8_t WIRE_MAGIC[4] = {'U', 'M', 'F', 'W'};
static const char WIRE_DOMAIN[] = "umicom.ai.federated-wire.schema-1";

enum {
    OFF_VERSION = 4, OFF_KIND = 6, OFF_LENGTH = 8,
    OFF_ROUND = 12, OFF_SEQUENCE = 20,
    OFF_CONTRIBUTOR = 28, OFF_BASE = 60, OFF_DATASET = 92,
    OFF_POLICY = 124, OFF_SAMPLES = 156, OFF_STEPS = 164,
    OFF_TRANSFER = 172, OFF_DELTA_COUNT = 180
};

static void Put16(uint8_t *buffer, uint16_t value)
{
    buffer[0] = (uint8_t)(value >> 8U);
    buffer[1] = (uint8_t)value;
}
static void Put32(uint8_t *buffer, uint32_t value)
{
    for (unsigned i = 0U; i < 4U; ++i)
        buffer[i] = (uint8_t)(value >> (24U - 8U * i));
}
static void Put64(uint8_t *buffer, uint64_t value)
{
    for (unsigned i = 0U; i < 8U; ++i)
        buffer[i] = (uint8_t)(value >> (56U - 8U * i));
}
static uint16_t Get16(const uint8_t *buffer)
{
    return (uint16_t)(((uint16_t)buffer[0] << 8U) | (uint16_t)buffer[1]);
}
static uint32_t Get32(const uint8_t *buffer)
{
    uint32_t value = 0U;
    for (unsigned i = 0U; i < 4U; ++i)
        value = (value << 8U) | (uint32_t)buffer[i];
    return value;
}
static uint64_t Get64(const uint8_t *buffer)
{
    uint64_t value = UINT64_C(0);
    for (unsigned i = 0U; i < 8U; ++i)
        value = (value << 8U) | (uint64_t)buffer[i];
    return value;
}
static int NonzeroDigest(const uint8_t *digest)
{
    unsigned nonzero = 0U;
    for (size_t i = 0U; i < UMI_AI_FEDERATED_DIGEST_BYTES; ++i)
        nonzero |= (unsigned)digest[i];
    return nonzero != 0U;
}
static int Overlap(const void *a, size_t an, const void *b, size_t bn)
{
    uintptr_t first = (uintptr_t)a;
    uintptr_t second = (uintptr_t)b;
    if (an == 0U || bn == 0U) return 0;
    if (first > UINTPTR_MAX - an || second > UINTPTR_MAX - bn) return 1;
    return first < second + bn && second < first + an;
}
static UmiStatus Checksum(const uint8_t *data, size_t size, uint8_t out[32])
{
    UmiSha256 ctx;
    UmiSha256Init(&ctx);
    UmiStatus status = UmiSha256Update(&ctx, WIRE_DOMAIN,
                                      sizeof(WIRE_DOMAIN) - 1U);
    if (status == UMI_STATUS_OK)
        status = UmiSha256Update(&ctx, data, size);
    if (status == UMI_STATUS_OK)
        status = UmiSha256Final(&ctx, out);
    return status;
}

static UmiStatus CheckRecord(const UmiAiTrainingFederatedWireRecord *record,
                             size_t *required)
{
    if (record == NULL || required == NULL ||
        record->roundId == UINT64_C(0) || record->sequence == UINT64_C(0) ||
        (record->kind != UMI_AI_FEDERATED_WIRE_ASSIGNMENT &&
         record->kind != UMI_AI_FEDERATED_WIRE_RESULT))
        return UMI_STATUS_INVALID_ARGUMENT;
    const UmiAiTrainingFederatedAssignment *a = &record->assignment;
    if (!NonzeroDigest(a->contributorId) ||
        !NonzeroDigest(a->baseCheckpointSha256) ||
        !NonzeroDigest(a->datasetCommitmentSha256) ||
        !NonzeroDigest(a->dataPolicySha256) ||
        a->distinctSampleCount == UINT64_C(0) ||
        a->localSteps == UINT64_C(0) ||
        a->declaredTransferBytes == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (record->kind == UMI_AI_FEDERATED_WIRE_ASSIGNMENT) {
        if (record->delta != NULL || record->deltaCount != 0U)
            return UMI_STATUS_INVALID_ARGUMENT;
    } else {
        if (record->delta == NULL || record->deltaCount == 0U ||
            record->deltaCount > UMI_AI_FEDERATED_MAX_PARAMETERS)
            return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t i = 0U; i < record->deltaCount; ++i)
            if (!isfinite(record->delta[i])) return UMI_STATUS_INVALID_ARGUMENT;
    }
    *required = UMI_AI_FEDERATED_WIRE_HEADER_BYTES +
        record->deltaCount * sizeof(float) +
        UMI_AI_FEDERATED_WIRE_CHECKSUM_BYTES;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingFederatedWireMeasure(
    const UmiAiTrainingFederatedWireRecord *record, size_t *outBytes)
{
    size_t required = 0U;
    if (outBytes == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = CheckRecord(record, &required);
    if (status == UMI_STATUS_OK) *outBytes = required;
    return status;
}

UmiStatus UmiAiTrainingFederatedWireEncode(
    const UmiAiTrainingFederatedWireRecord *record,
    uint8_t *outFrame, size_t frameCapacity, size_t *outFrameBytes)
{
    size_t bytes = 0U;
    if (outFrame == NULL || outFrameBytes == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = CheckRecord(record, &bytes);
    if (status != UMI_STATUS_OK) return status;
    if (frameCapacity < bytes) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (Overlap(outFrame, bytes, record, sizeof(*record)) ||
        Overlap(outFrame, bytes, outFrameBytes, sizeof(*outFrameBytes)) ||
        Overlap(outFrame, bytes, record->delta,
                record->deltaCount * sizeof(float)))
        return UMI_STATUS_INVALID_ARGUMENT;

    /* Validation and SHA-256 finish in local staging before one copy.
     * No partial frame can escape if an internal calculation fails. */
    uint8_t staging[UMI_AI_FEDERATED_WIRE_MAX_BYTES] = {0};
    memcpy(staging, WIRE_MAGIC, sizeof(WIRE_MAGIC));
    Put16(staging + OFF_VERSION, 1U);
    Put16(staging + OFF_KIND, (uint16_t)record->kind);
    Put32(staging + OFF_LENGTH, (uint32_t)bytes);
    Put64(staging + OFF_ROUND, record->roundId);
    Put64(staging + OFF_SEQUENCE, record->sequence);
    const UmiAiTrainingFederatedAssignment *a = &record->assignment;
    memcpy(staging + OFF_CONTRIBUTOR, a->contributorId, 32U);
    memcpy(staging + OFF_BASE, a->baseCheckpointSha256, 32U);
    memcpy(staging + OFF_DATASET, a->datasetCommitmentSha256, 32U);
    memcpy(staging + OFF_POLICY, a->dataPolicySha256, 32U);
    Put64(staging + OFF_SAMPLES, a->distinctSampleCount);
    Put64(staging + OFF_STEPS, a->localSteps);
    Put64(staging + OFF_TRANSFER, (uint64_t)a->declaredTransferBytes);
    Put32(staging + OFF_DELTA_COUNT, (uint32_t)record->deltaCount);
    for (size_t i = 0U; i < record->deltaCount; ++i) {
        uint32_t bits = 0U;
        memcpy(&bits, &record->delta[i], sizeof(bits));
        Put32(staging + UMI_AI_FEDERATED_WIRE_HEADER_BYTES + 4U * i, bits);
    }
    status = Checksum(staging, bytes - 32U, staging + bytes - 32U);
    if (status != UMI_STATUS_OK) return status;
    memcpy(outFrame, staging, bytes);
    *outFrameBytes = bytes;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingFederatedWireDecode(
    const uint8_t *frame, size_t frameBytes,
    UmiAiTrainingFederatedWireRecord *outRecord,
    float *outDelta, size_t deltaCapacity)
{
    if (frame == NULL || outRecord == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (frameBytes < UMI_AI_FEDERATED_WIRE_HEADER_BYTES + 32U ||
        frameBytes > UMI_AI_FEDERATED_WIRE_MAX_BYTES)
        return UMI_STATUS_PARSE_ERROR;
    if (Overlap(frame, frameBytes, outRecord, sizeof(*outRecord)))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (memcmp(frame, WIRE_MAGIC, sizeof(WIRE_MAGIC)) != 0 ||
        Get16(frame + OFF_VERSION) != 1U ||
        Get32(frame + OFF_LENGTH) != frameBytes)
        return UMI_STATUS_PARSE_ERROR;
    const uint16_t kind = Get16(frame + OFF_KIND);
    const uint32_t count = Get32(frame + OFF_DELTA_COUNT);
    if ((kind != (uint16_t)UMI_AI_FEDERATED_WIRE_ASSIGNMENT &&
         kind != (uint16_t)UMI_AI_FEDERATED_WIRE_RESULT) ||
        count > UMI_AI_FEDERATED_MAX_PARAMETERS ||
        (kind == (uint16_t)UMI_AI_FEDERATED_WIRE_ASSIGNMENT && count != 0U) ||
        (kind == (uint16_t)UMI_AI_FEDERATED_WIRE_RESULT && count == 0U) ||
        frameBytes != UMI_AI_FEDERATED_WIRE_HEADER_BYTES + 4U * (size_t)count + 32U)
        return UMI_STATUS_PARSE_ERROR;
    if (count > 0U && outDelta == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (count > deltaCapacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (count > 0U &&
        (Overlap(frame, frameBytes, outDelta, (size_t)count * sizeof(float)) ||
         Overlap(outRecord, sizeof(*outRecord), outDelta,
                 (size_t)count * sizeof(float))))
        return UMI_STATUS_INVALID_ARGUMENT;
    uint8_t expected[32];
    UmiStatus status = Checksum(frame, frameBytes - 32U, expected);
    if (status != UMI_STATUS_OK) return status;
    if (memcmp(expected, frame + frameBytes - 32U, 32U) != 0)
        return UMI_STATUS_PARSE_ERROR;

    UmiAiTrainingFederatedWireRecord decoded = {0};
    decoded.kind = (UmiAiTrainingFederatedWireKind)kind;
    decoded.roundId = Get64(frame + OFF_ROUND);
    decoded.sequence = Get64(frame + OFF_SEQUENCE);
    UmiAiTrainingFederatedAssignment *a = &decoded.assignment;
    memcpy(a->contributorId, frame + OFF_CONTRIBUTOR, 32U);
    memcpy(a->baseCheckpointSha256, frame + OFF_BASE, 32U);
    memcpy(a->datasetCommitmentSha256, frame + OFF_DATASET, 32U);
    memcpy(a->dataPolicySha256, frame + OFF_POLICY, 32U);
    a->distinctSampleCount = Get64(frame + OFF_SAMPLES);
    a->localSteps = Get64(frame + OFF_STEPS);
    uint64_t declared = Get64(frame + OFF_TRANSFER);
    if (declared > (uint64_t)SIZE_MAX) return UMI_STATUS_PARSE_ERROR;
    a->declaredTransferBytes = (size_t)declared;
    decoded.deltaCount = (size_t)count;
    float staged[UMI_AI_FEDERATED_MAX_PARAMETERS];
    for (size_t i = 0U; i < decoded.deltaCount; ++i) {
        uint32_t bits = Get32(frame + UMI_AI_FEDERATED_WIRE_HEADER_BYTES + 4U * i);
        memcpy(&staged[i], &bits, sizeof(bits));
        if (!isfinite(staged[i])) return UMI_STATUS_PARSE_ERROR;
    }
    /* Validate against the staged floats, never uninitialised caller output. */
    decoded.delta = decoded.deltaCount > 0U ? staged : NULL;
    size_t required = 0U;
    status = CheckRecord(&decoded, &required);
    if (status != UMI_STATUS_OK || required != frameBytes)
        return UMI_STATUS_PARSE_ERROR;
    if (decoded.deltaCount > 0U) {
        memcpy(outDelta, staged, decoded.deltaCount * sizeof(float));
        decoded.delta = outDelta;
    }
    *outRecord = decoded;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingFederatedWireMatchResult(
    const UmiAiTrainingFederatedWireRecord *result,
    const UmiAiTrainingFederatedAssignment *admitted,
    uint64_t roundId, uint64_t sequence)
{
    if (result == NULL || admitted == NULL || roundId == UINT64_C(0) ||
        sequence == UINT64_C(0)) return UMI_STATUS_INVALID_ARGUMENT;
    if (result->kind != UMI_AI_FEDERATED_WIRE_RESULT ||
        result->roundId != roundId || result->sequence != sequence ||
        memcmp(result->assignment.contributorId, admitted->contributorId, 32U) != 0 ||
        memcmp(result->assignment.baseCheckpointSha256,
               admitted->baseCheckpointSha256, 32U) != 0 ||
        memcmp(result->assignment.datasetCommitmentSha256,
               admitted->datasetCommitmentSha256, 32U) != 0 ||
        memcmp(result->assignment.dataPolicySha256,
               admitted->dataPolicySha256, 32U) != 0 ||
        result->assignment.distinctSampleCount != admitted->distinctSampleCount ||
        result->assignment.localSteps != admitted->localSteps ||
        result->assignment.declaredTransferBytes != admitted->declaredTransferBytes)
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingFederatedWireContribution(
    const UmiAiTrainingFederatedWireRecord *result,
    UmiAiTrainingFederatedContribution *outContribution)
{
    if (result == NULL || outContribution == NULL ||
        result->kind != UMI_AI_FEDERATED_WIRE_RESULT ||
        result->delta == NULL || result->deltaCount == 0U ||
        result->deltaCount > UMI_AI_FEDERATED_MAX_PARAMETERS)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiAiTrainingFederatedContribution contribution = {0};
    memcpy(contribution.contributorId, result->assignment.contributorId, 32U);
    memcpy(contribution.baseCheckpointSha256,
           result->assignment.baseCheckpointSha256, 32U);
    memcpy(contribution.datasetCommitmentSha256,
           result->assignment.datasetCommitmentSha256, 32U);
    contribution.distinctSampleCount = result->assignment.distinctSampleCount;
    contribution.weightSpaceDelta = result->delta;
    contribution.deltaCount = result->deltaCount;
    *outContribution = contribution;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingFederatedWireBindEnvelope(
    const uint8_t *frame, size_t frameBytes, UmiMessageEnvelope *outEnvelope)
{
    if (frame == NULL || outEnvelope == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (Overlap(frame, frameBytes, outEnvelope, sizeof(*outEnvelope)))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiAiTrainingFederatedWireRecord decoded = {0};
    float temporary[UMI_AI_FEDERATED_MAX_PARAMETERS];
    UmiStatus status = UmiAiTrainingFederatedWireDecode(frame, frameBytes,
        &decoded, temporary, UMI_AI_FEDERATED_MAX_PARAMETERS);
    if (status != UMI_STATUS_OK) return status;
    UmiMessageEnvelope message = {0};
    message.structure_size = (uint32_t)sizeof(message);
    message.schema_version = 1U;
    message.kind = decoded.kind == UMI_AI_FEDERATED_WIRE_ASSIGNMENT ?
        UMI_MESSAGE_JOB : UMI_MESSAGE_REPLY;
    message.name = decoded.kind == UMI_AI_FEDERATED_WIRE_ASSIGNMENT ?
        "umicom.ai.federated.assignment" : "umicom.ai.federated.result";
    message.payload = "";
    message.schema_id = "org.umicom.ai.federated.wire";
    message.payload_data = frame;
    message.payload_size = frameBytes;
    message.sequence = decoded.sequence;
    message.correlation_id = decoded.roundId;
    message.flags = UMI_MESSAGE_FLAG_IDEMPOTENT;
    *outEnvelope = message;
    return UMI_STATUS_OK;
}
