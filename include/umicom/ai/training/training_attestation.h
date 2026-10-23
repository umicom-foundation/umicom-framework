/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/training_attestation.h
 *
 * PURPOSE:
 *   Sign a previously verified native training publication, or independently
 *   verify a detached Ed25519 signature against an explicitly trusted key.
 *
 * SECURITY AND ARCHITECTURE:
 *   This optional C23 service reuses Framework publication verification and
 *   the canonical SHA-256 implementation. The Ed25519 primitive is provided
 *   by OpenSSL libcrypto (optional, never enabled in default builds).
 *   Trust must come from a PUBLIC KEY chosen independently of the signature
 *   or checkpoint. A signature never proves model accuracy, dataset consent,
 *   a safe build host, or the authority of an unreviewed public key.
 *
 *   Signing is an explicit, separately authorised operation. The key remains
 *   outside the repository and is read only for the duration of that call.
 *   Signatures are written to a NEW, no-clobber companion pathname. No
 *   original checkpoint, publication record or source data is modified.
 *   Filesystem races are not a sandbox: use immutable reviewed evidence on
 *   a controlled host and reverify immediately before downstream use.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_TRAINING_ATTESTATION_H
#define UMICOM_AI_TRAINING_TRAINING_ATTESTATION_H

#include "umicom/ai/training/training_publication.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_TRAINING_ATTESTATION_RECORD_BYTES 254U
#define UMI_AI_TRAINING_ATTESTATION_SUFFIX ".publication.ed25519"

/* Only call after a security review of the host and published bundle.
 * The private PEM key must be an Ed25519 key held outside Git. The optional
 * OpenSSL provider may prompt for a password if the PEM is encrypted.
 * A pre-existing destination or left-over staging file is never overwritten.
 * Invalid input does not create a signature or alter any source evidence. */
UmiStatus UmiAiTrainingAttestationSignNewFile(
    const char *checkpointPath, const char *privateKeyPemPath);

/* A caller-approved, independently distributed Ed25519 PUBLIC key is
 * mandatory. The verification first authenticates the exact publication
 * record bytes, then rehashes the four files using the existing Framework
 * publication verifier. A self-declared key inside the evidence is NEVER
 * trusted as a substitute for trustedPublicKeyPemPath. */
UmiStatus UmiAiTrainingAttestationVerifyFile(
    const char *checkpointPath, const char *trustedPublicKeyPemPath);

/* Same checks, but publish matching copied file identities only on success.
 * Any validation or I/O error leaves *outEvidence completely unchanged. */
UmiStatus UmiAiTrainingAttestationVerifyAndInspectFile(
    const char *checkpointPath, const char *trustedPublicKeyPemPath,
    UmiAiTrainingPublicationEvidence *outEvidence);

/* Derive the SHA-256 fingerprint of the raw 32-byte Ed25519 public key,
 * not the text of its PEM container. This is an independent provisioning
 * aid for preparing reviewed trust-policy records. The supplied PEM must be
 * a valid Ed25519 public key. Errors preserve the caller's output buffer;
 * obtaining a fingerprint never automatically authorises that key. */
UmiStatus UmiAiTrainingAttestationPublicKeyFingerprint(
    const char *publicKeyPemPath,
    char outFingerprint[UMI_SHA256_HEX_CAPACITY]);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_TRAINING_ATTESTATION_H */
