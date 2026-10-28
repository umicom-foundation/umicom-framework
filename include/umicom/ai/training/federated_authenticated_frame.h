/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/federated_authenticated_frame.h
 *
 * PURPOSE:
 *   Bind the existing portable federated wire frame to a keyed HMAC-SHA-256
 *   integrity envelope while retaining the canonical decoder and job owner.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_FEDERATED_AUTHENTICATED_FRAME_H
#define UMICOM_AI_TRAINING_FEDERATED_AUTHENTICATED_FRAME_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/federated_wire.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_FEDERATED_AUTH_HEADER_BYTES 20U
#define UMI_AI_FEDERATED_AUTH_TAG_BYTES 32U
#define UMI_AI_FEDERATED_AUTH_MAX_BYTES \
    (UMI_AI_FEDERATED_AUTH_HEADER_BYTES + \
     UMI_AI_FEDERATED_WIRE_MAX_BYTES + UMI_AI_FEDERATED_AUTH_TAG_BYTES)

/* Frame: 'UMFA'[4], schema=1[2], flags=0[2], keyEpoch[8],
 * innerWireLength[4], exact EXISTING federated wire bytes, HMAC-SHA-256[32].
 * All integer fields are network byte order. The HMAC covers the complete
 * header and inner frame, excluding only the 32-byte MAC itself.
 * A caller-provided key epoch labels secret rotation; the receiver must have
 * an independently authorised binding of worker/epoch to secret material.
 * This is shared-secret MAC authentication, NOT asymmetric signatures,
 * transport encryption, replay prevention, honest-gradient verification,
 * legal dataset authorisation, or blockchain consensus. No key is stored.
 * Callers must use a high-entropy, securely provisioned per-worker secret.
 * Failure of every API preserves all caller-owned output storage. */
UmiStatus UmiAiTrainingFederatedAuthSeal(
    const uint8_t *wireFrame, size_t wireBytes,
    uint64_t keyEpoch, const uint8_t *secret, size_t secretBytes,
    uint8_t *outAuthenticated, size_t capacity, size_t *outBytes);

/* Decode the existing inner wire ONLY AFTER MAC verification. Expected epoch
 * guards against rotated-key fallback. The returned delta pointer aliases
 * caller-owned outDelta, exactly as in the established wire decoder. */
UmiStatus UmiAiTrainingFederatedAuthOpen(
    const uint8_t *authenticated, size_t authenticatedBytes,
    uint64_t expectedEpoch, const uint8_t *secret, size_t secretBytes,
    UmiAiTrainingFederatedWireRecord *outRecord,
    float *outDelta, size_t deltaCapacity);

/* Create a BORROWED typed Framework message envelope from an authenticated
 * frame. Its payload pointer refers to authenticated; call umi_message_copy
 * before queueing it or before releasing that buffer. Unseal again at the
 * receiving trust boundary, then authorise identity and map the local ticket. */
UmiStatus UmiAiTrainingFederatedAuthBindEnvelope(
    const uint8_t *authenticated, size_t authenticatedBytes,
    uint64_t expectedEpoch, const uint8_t *secret, size_t secretBytes,
    UmiMessageEnvelope *outEnvelope);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_FEDERATED_AUTHENTICATED_FRAME_H */
