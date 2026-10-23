/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/training_trust_receipt.c
 *
 * PURPOSE:
 *   Provide failure-atomic machine-readable evidence of an independently
 *   verified Ed25519 native-training publication and approved publisher.
 *
 * IMPLEMENTATION:
 *   Verify through the existing Framework trust authority first; never
 *   invent a successful result from a caller-supplied structure. JSON keys
 *   and all string-valued fields are fixed ASCII or validated lower-case
 *   hexadecimal fingerprints, so no generic escaping/parser is necessary.
 *   The complete response is staged on the stack and committed together.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/training_trust_receipt.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

static int IsLowerHexDigest(const char value[UMI_SHA256_HEX_CAPACITY])
{
    if (value == NULL) return 0;
    for (size_t i = 0U; i < UMI_SHA256_BYTES * 2U; ++i) {
        const unsigned char byte = (unsigned char)value[i];
        if (!((byte >= '0' && byte <= '9') ||
              (byte >= 'a' && byte <= 'f'))) return 0;
    }
    return value[UMI_SHA256_BYTES * 2U] == '\0';
}

static int HasVerifiedIdentity(const UmiAiTrainingPublicationFileIdentity *file)
{
    return file != NULL && file->byteCount != UINT64_C(0) &&
           IsLowerHexDigest(file->sha256);
}

UmiStatus UmiAiTrainingTrustReceiptVerifyJson(
    const char *checkpointPath,
    const char *independentlyApprovedPolicyFile,
    uint64_t verificationUnixSeconds,
    char *outJson, size_t outputCapacity, size_t *outWritten)
{
    UmiAiTrainingTrustDecision verified;
    const UmiAiTrainingPublicationEvidence *publication;
    char staged[UMI_AI_TRAINING_TRUST_RECEIPT_JSON_CAPACITY];
    UmiStatus status;
    int written;

    if (checkpointPath == NULL || checkpointPath[0] == '\0' ||
        independentlyApprovedPolicyFile == NULL ||
        independentlyApprovedPolicyFile[0] == '\0' ||
        outJson == NULL || outWritten == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;

    /* The existing, optional OpenSSL-backed Framework attestation verifies
     * the policy, active key, Ed25519 signature AND all four saved files.
     * It publishes its copied evidence only when every check passes. */
    status = UmiAiTrainingAttestationVerifyWithTrustPolicy(
        checkpointPath, independentlyApprovedPolicyFile,
        verificationUnixSeconds, &verified);
    if (status != UMI_STATUS_OK) return status;

    publication = &verified.publication;
    if (!IsLowerHexDigest(verified.verifiedPublicKeySha256) ||
        verified.evaluatedAtUnixSeconds != verificationUnixSeconds ||
        !HasVerifiedIdentity(&publication->checkpoint) ||
        !HasVerifiedIdentity(&publication->clipPolicy) ||
        !HasVerifiedIdentity(&publication->trainingLock) ||
        !HasVerifiedIdentity(&publication->sourceLock))
        return UMI_STATUS_INVALID_STATE;

    /* This stable schema contains no checkpoint or policy path and does not
     * echo arbitrary input. A receipt documents a successful local check,
     * not a signed proof that the check ran on a trustworthy host. */
    written = snprintf(
        staged, sizeof(staged),
        "{\"schema\":\"umicom.ai.training.trust-receipt\","
        "\"result\":\"verified\","
        "\"evaluated_at_unix_seconds\":%" PRIu64 ","
        "\"publisher_key_sha256\":\"%s\","
        "\"files\":{"
          "\"checkpoint\":{\"bytes\":%" PRIu64 ",\"sha256\":\"%s\"},"
          "\"clip_policy\":{\"bytes\":%" PRIu64 ",\"sha256\":\"%s\"},"
          "\"training_lock\":{\"bytes\":%" PRIu64 ",\"sha256\":\"%s\"},"
          "\"source_lock\":{\"bytes\":%" PRIu64 ",\"sha256\":\"%s\"}"
        "},"
        "\"checks\":{"
          "\"ed25519_signature\":true,"
          "\"approved_publisher_at_evaluation_time\":true,"
          "\"publication_file_integrity\":true,"
          "\"trusted_signing_timestamp\":false,"
          "\"host_integrity_attested\":false,"
          "\"dataset_rights_reviewed\":false"
        "}}",
        verified.evaluatedAtUnixSeconds,
        verified.verifiedPublicKeySha256,
        publication->checkpoint.byteCount, publication->checkpoint.sha256,
        publication->clipPolicy.byteCount, publication->clipPolicy.sha256,
        publication->trainingLock.byteCount, publication->trainingLock.sha256,
        publication->sourceLock.byteCount, publication->sourceLock.sha256);

    if (written < 0 || (size_t)written >= sizeof(staged))
        return UMI_STATUS_INTERNAL_ERROR;
    if ((size_t)written >= outputCapacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    memcpy(outJson, staged, (size_t)written + 1U);
    *outWritten = (size_t)written;
    return UMI_STATUS_OK;
}
