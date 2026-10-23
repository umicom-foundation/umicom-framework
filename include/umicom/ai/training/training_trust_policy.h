/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/training_trust_policy.h
 *
 * PURPOSE:
 *   Read a locally approved Ed25519 publisher trust policy and verify an
 *   existing, signed native training publication under that policy.
 *
 * ARCHITECTURE AND TRUST:
 *   This optional C23 service composes the Framework's existing Ed25519
 *   signature verifier and publication-integrity verifier. It does not
 *   duplicate signature mathematics, load model weights, or run training.
 *   The policy path MUST be chosen independently of the model/signature.
 *   A policy file is a local trust anchor, NOT a self-authenticating record:
 *   protect its contents, directory and public-key files from unauthorised
 *   replacement. Symlinks and filesystem races are not sandboxed.
 *
 *   Availability intervals are evaluated at the caller-supplied UTC Unix
 *   second. In the absence of a trusted signing timestamp these intervals
 *   say nothing about when the signature was created. A revoked key is never
 *   accepted, even when inspecting an older signature at an earlier time.
 *   This is an offline verification policy, not a certificate authority,
 *   transparency log, revocation-distribution service or HSM integration.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_TRAINING_TRUST_POLICY_H
#define UMICOM_AI_TRAINING_TRAINING_TRUST_POLICY_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/training_attestation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_TRAINING_TRUST_MAX_FILE_BYTES 8192U
#define UMI_AI_TRAINING_TRUST_MAX_KEYS 16U
#define UMI_AI_TRAINING_TRUST_MAX_RELATIVE_KEY_PATH 160U

typedef enum UmiAiTrainingTrustKeyState {
    UMI_AI_TRAINING_TRUST_KEY_ACTIVE = 1,
    UMI_AI_TRAINING_TRUST_KEY_REVOKED = 2
} UmiAiTrainingTrustKeyState;

typedef struct UmiAiTrainingTrustKey {
    char publicKeySha256[UMI_SHA256_HEX_CAPACITY];
    char relativePemPath[UMI_AI_TRAINING_TRUST_MAX_RELATIVE_KEY_PATH];
    uint64_t validFromUnixSeconds;
    uint64_t validUntilUnixSeconds; /* exclusive */
    UmiAiTrainingTrustKeyState state;
} UmiAiTrainingTrustKey;

typedef struct UmiAiTrainingTrustPolicy {
    size_t keyCount;
    UmiAiTrainingTrustKey keys[UMI_AI_TRAINING_TRUST_MAX_KEYS];
} UmiAiTrainingTrustPolicy;

typedef struct UmiAiTrainingTrustDecision {
    char verifiedPublicKeySha256[UMI_SHA256_HEX_CAPACITY];
    uint64_t evaluatedAtUnixSeconds;
    UmiAiTrainingPublicationEvidence publication;
} UmiAiTrainingTrustDecision;

/* Strict ASCII input, with LF/CRLF, optional blank/comment lines and no
 * interpolation, quoting, escapes or external includes:
 *
 *   UMICOM-AI-TRUST-1
 *   key=<64 lowercase SHA256 hex>,keys/member.pem,<from>,<until>,active
 *   key=<64 lowercase SHA256 hex>,keys/old.pem,<from>,<until>,revoked
 *
 * Fingerprints are SHA-256 of the RAW 32-byte Ed25519 public key (not PEM
 * text). Times are decimal UTC Unix seconds; from is inclusive and until
 * exclusive. Each exact fingerprint occurs at most once. Relative PEM paths
 * reject '.', '..', empty components and nonportable characters. They are
 * resolved relative to the policy FILE, not the current working directory.
 * No output is modified when parsing/loading fails. */
UmiStatus UmiAiTrainingTrustPolicyParse(
    const char *text, size_t textLength,
    UmiAiTrainingTrustPolicy *outPolicy);

UmiStatus UmiAiTrainingTrustPolicyLoadFile(
    const char *policyFile, UmiAiTrainingTrustPolicy *outPolicy);

/* Read the detached signature's claimed key fingerprint ONLY as a selector
 * into the independently reviewed allowlist. Enforce revocation and current
 * validity interval BEFORE passing the independently trusted PEM path to the
 * existing canonical Ed25519 verifier. The canonical verifier must then
 * check the key fingerprint, cryptographic signature and published bytes.
 * Never treat a claim from the signature as a new trust anchor.
 * On failure *outDecision is byte-for-byte unchanged. */
UmiStatus UmiAiTrainingAttestationVerifyWithTrustPolicy(
    const char *checkpointPath, const char *policyFile,
    uint64_t verificationUnixSeconds,
    UmiAiTrainingTrustDecision *outDecision);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_TRAINING_TRUST_POLICY_H */
