/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/federated_authenticated_frame.c
 *
 * PURPOSE:
 *   Validate and HMAC-authenticate bounded federated wire frames using
 *   Framework-owned SHA-256, existing wire decoding and typed envelopes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/federated_authenticated_frame.h"
#include "umicom/security/hmac_sha256.h"

#include <stdint.h>
#include <string.h>

static const uint8_t AUTH_MAGIC[4] = {'U', 'M', 'F', 'A'};

static int AuthOverlaps(const void *a, size_t an,
                        const void *b, size_t bn)
{
    uintptr_t first = (uintptr_t)a, second = (uintptr_t)b;
    if (an == 0U || bn == 0U) return 0;
    if (first > UINTPTR_MAX - an || second > UINTPTR_MAX - bn) return 1;
    return first < second + bn && second < first + an;
}
static void AuthPut16(uint8_t *out, uint16_t value)
{
    out[0] = (uint8_t)(value >> 8U);
    out[1] = (uint8_t)value;
}
static void AuthPut32(uint8_t *out, uint32_t value)
{
    for (unsigned index = 0U; index < 4U; ++index)
        out[index] = (uint8_t)(value >> (24U - index * 8U));
}
static void AuthPut64(uint8_t *out, uint64_t value)
{
    for (unsigned index = 0U; index < 8U; ++index)
        out[index] = (uint8_t)(value >> (56U - index * 8U));
}
static uint16_t AuthGet16(const uint8_t *data)
{
    return (uint16_t)(((uint16_t)data[0] << 8U) | (uint16_t)data[1]);
}
static uint32_t AuthGet32(const uint8_t *data)
{
    uint32_t value = 0U;
    for (unsigned index = 0U; index < 4U; ++index)
        value = (value << 8U) | (uint32_t)data[index];
    return value;
}
static uint64_t AuthGet64(const uint8_t *data)
{
    uint64_t value = UINT64_C(0);
    for (unsigned index = 0U; index < 8U; ++index)
        value = (value << 8U) | (uint64_t)data[index];
    return value;
}

static UmiStatus AuthParseHeader(const uint8_t *frame, size_t totalBytes,
                                 uint64_t expectedEpoch, size_t *innerBytes)
{
    if (frame == NULL || innerBytes == NULL || expectedEpoch == UINT64_C(0))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (totalBytes < UMI_AI_FEDERATED_AUTH_HEADER_BYTES +
                     UMI_AI_FEDERATED_WIRE_HEADER_BYTES +
                     UMI_AI_FEDERATED_WIRE_CHECKSUM_BYTES +
                     UMI_AI_FEDERATED_AUTH_TAG_BYTES ||
        totalBytes > UMI_AI_FEDERATED_AUTH_MAX_BYTES)
        return UMI_STATUS_PARSE_ERROR;
    if (memcmp(frame, AUTH_MAGIC, sizeof(AUTH_MAGIC)) != 0 ||
        AuthGet16(frame + 4U) != 1U || AuthGet16(frame + 6U) != 0U)
        return UMI_STATUS_PARSE_ERROR;
    if (AuthGet64(frame + 8U) != expectedEpoch)
        return UMI_STATUS_PERMISSION_DENIED;
    const size_t inner = (size_t)AuthGet32(frame + 16U);
    if (inner > UMI_AI_FEDERATED_WIRE_MAX_BYTES ||
        inner < UMI_AI_FEDERATED_WIRE_HEADER_BYTES +
                UMI_AI_FEDERATED_WIRE_CHECKSUM_BYTES ||
        inner + UMI_AI_FEDERATED_AUTH_HEADER_BYTES +
            UMI_AI_FEDERATED_AUTH_TAG_BYTES != totalBytes)
        return UMI_STATUS_PARSE_ERROR;
    *innerBytes = inner;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingFederatedAuthSeal(
    const uint8_t *wireFrame, size_t wireBytes,
    uint64_t keyEpoch, const uint8_t *secret, size_t secretBytes,
    uint8_t *outAuthenticated, size_t capacity, size_t *outBytes)
{
    if (wireFrame == NULL || secret == NULL || secretBytes == 0U ||
        outAuthenticated == NULL || outBytes == NULL || keyEpoch == UINT64_C(0))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (wireBytes < UMI_AI_FEDERATED_WIRE_HEADER_BYTES +
                    UMI_AI_FEDERATED_WIRE_CHECKSUM_BYTES ||
        wireBytes > UMI_AI_FEDERATED_WIRE_MAX_BYTES)
        return UMI_STATUS_INVALID_ARGUMENT;
    const size_t total = UMI_AI_FEDERATED_AUTH_HEADER_BYTES + wireBytes +
                         UMI_AI_FEDERATED_AUTH_TAG_BYTES;
    if (capacity < total) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (AuthOverlaps(outAuthenticated, total, wireFrame, wireBytes) ||
        AuthOverlaps(outBytes, sizeof(*outBytes), wireFrame, wireBytes) ||
        AuthOverlaps(outAuthenticated, total, secret, secretBytes) ||
        AuthOverlaps(outAuthenticated, total, outBytes, sizeof(*outBytes)))
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Validate the canonical wire BEFORE sealing. This prevents authenticating
     * malformed or non-finite update vectors even under an approved key. */
    UmiAiTrainingFederatedWireRecord record = {0};
    float values[UMI_AI_FEDERATED_MAX_PARAMETERS] = {0};
    UmiStatus status = UmiAiTrainingFederatedWireDecode(wireFrame, wireBytes,
        &record, values, UMI_AI_FEDERATED_MAX_PARAMETERS);
    if (status != UMI_STATUS_OK) return status;
    uint8_t prepared[UMI_AI_FEDERATED_AUTH_MAX_BYTES] = {0};
    memcpy(prepared, AUTH_MAGIC, sizeof(AUTH_MAGIC));
    AuthPut16(prepared + 4U, UINT16_C(1));
    AuthPut16(prepared + 6U, UINT16_C(0));
    AuthPut64(prepared + 8U, keyEpoch);
    AuthPut32(prepared + 16U, (uint32_t)wireBytes);
    memcpy(prepared + UMI_AI_FEDERATED_AUTH_HEADER_BYTES, wireFrame, wireBytes);
    status = UmiSecurityHmacSha256Compute(secret, secretBytes, prepared,
        total - UMI_AI_FEDERATED_AUTH_TAG_BYTES,
        prepared + total - UMI_AI_FEDERATED_AUTH_TAG_BYTES);
    if (status != UMI_STATUS_OK) return status;
    memcpy(outAuthenticated, prepared, total);
    *outBytes = total;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingFederatedAuthOpen(
    const uint8_t *authenticated, size_t authenticatedBytes,
    uint64_t expectedEpoch, const uint8_t *secret, size_t secretBytes,
    UmiAiTrainingFederatedWireRecord *outRecord,
    float *outDelta, size_t deltaCapacity)
{
    if (authenticated == NULL || secret == NULL || secretBytes == 0U ||
        outRecord == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (deltaCapacity > SIZE_MAX / sizeof(float))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (AuthOverlaps(secret, secretBytes, outRecord, sizeof(*outRecord)) ||
        (outDelta != NULL && AuthOverlaps(secret, secretBytes, outDelta,
                                        deltaCapacity * sizeof(float))))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (AuthOverlaps(authenticated, authenticatedBytes, outRecord,
                     sizeof(*outRecord)) ||
        (outDelta != NULL && AuthOverlaps(authenticated, authenticatedBytes,
                  outDelta, deltaCapacity * sizeof(float))))
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t inner = 0U;
    UmiStatus status = AuthParseHeader(authenticated, authenticatedBytes,
                                      expectedEpoch, &inner);
    if (status != UMI_STATUS_OK) return status;
    status = UmiSecurityHmacSha256Verify(secret, secretBytes, authenticated,
        authenticatedBytes - UMI_AI_FEDERATED_AUTH_TAG_BYTES,
        authenticated + authenticatedBytes - UMI_AI_FEDERATED_AUTH_TAG_BYTES);
    if (status != UMI_STATUS_OK) return status;
    return UmiAiTrainingFederatedWireDecode(
        authenticated + UMI_AI_FEDERATED_AUTH_HEADER_BYTES, inner,
        outRecord, outDelta, deltaCapacity);
}

UmiStatus UmiAiTrainingFederatedAuthBindEnvelope(
    const uint8_t *authenticated, size_t authenticatedBytes,
    uint64_t expectedEpoch, const uint8_t *secret, size_t secretBytes,
    UmiMessageEnvelope *outEnvelope)
{
    if (outEnvelope == NULL || authenticated == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (AuthOverlaps(authenticated, authenticatedBytes,
                     outEnvelope, sizeof(*outEnvelope)))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiAiTrainingFederatedWireRecord record = {0};
    float temporary[UMI_AI_FEDERATED_MAX_PARAMETERS] = {0};
    UmiStatus status = UmiAiTrainingFederatedAuthOpen(authenticated,
        authenticatedBytes, expectedEpoch, secret, secretBytes,
        &record, temporary, UMI_AI_FEDERATED_MAX_PARAMETERS);
    if (status != UMI_STATUS_OK) return status;
    /* Preserve the original Framework envelope's field defaults, IDs and
     * routing conventions; substitute only the authenticated payload schema. */
    UmiMessageEnvelope candidate = {0};
    const uint8_t *innerFrame = authenticated +
                                UMI_AI_FEDERATED_AUTH_HEADER_BYTES;
    const size_t innerBytes = authenticatedBytes -
        UMI_AI_FEDERATED_AUTH_HEADER_BYTES - UMI_AI_FEDERATED_AUTH_TAG_BYTES;
    status = UmiAiTrainingFederatedWireBindEnvelope(innerFrame, innerBytes,
                                                     &candidate);
    if (status != UMI_STATUS_OK) return status;
    candidate.name = "umicom.ai.federated.authenticated";
    candidate.schema_id = "org.umicom.ai.federated.authenticated-frame";
    candidate.payload_data = authenticated;
    candidate.payload_size = authenticatedBytes;
    *outEnvelope = candidate;
    return UMI_STATUS_OK;
}
