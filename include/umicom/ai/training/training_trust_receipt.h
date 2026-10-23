/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/training_trust_receipt.h
 *
 * PURPOSE:
 *   Convert successful offline Ed25519 publisher verification into a bounded,
 *   machine-readable C23 JSON receipt for independent release and audit tools.
 *
 * ARCHITECTURE AND TRUST:
 *   The existing Framework trust-policy, Ed25519 attestation and publication
 *   verifier remain the ONLY authorities for acceptance. This service MUST
 *   verify first and serialise only a successfully verified, copied decision.
 *   No model is decoded or trained, no checkpoint is modified, no network
 *   connection is made and no private signing key is required.
 *
 *   A receipt is an UNSIGNED observation, not an attestation or trusted
 *   timestamp. It can be edited independently. UTC seconds are supplied by
 *   the caller, and cannot establish when an Ed25519 signature was created.
 *   Protect signed source evidence, trust-policy files and the machine that
 *   performs verification. Authorisation and model correctness remain
 *   independent review decisions.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_TRAINING_TRUST_RECEIPT_H
#define UMICOM_AI_TRAINING_TRAINING_TRUST_RECEIPT_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/training_trust_policy.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Upper bound for one canonical, single-line UTF-8/ASCII JSON receipt,
 * including its trailing NUL. No secret, path, or untrusted text is echoed. */
#define UMI_AI_TRAINING_TRUST_RECEIPT_JSON_CAPACITY 2048U

/* Verify the signed publication against an independently selected trust
 * policy at an explicitly provided UTC Unix second, then format one stable,
 * newline-free JSON object containing only copied, verified identities.
 *
 * A successful call writes a NUL-terminated JSON string and *outWritten
 * (length WITHOUT the NUL). Both outputs are preserved byte-for-byte on ANY
 * failure, including invalid arguments, refused trust and insufficient
 * capacity. There is no size-only mode; supply a buffer of the published
 * capacity. The result is not a cryptographically signed document.
 *
 * The caller must separately preserve the signed .publication.ed25519,
 * original evidence files, its trusted policy and verification environment. */
UmiStatus UmiAiTrainingTrustReceiptVerifyJson(
    const char *checkpointPath,
    const char *independentlyApprovedPolicyFile,
    uint64_t verificationUnixSeconds,
    char *outJson,
    size_t outputCapacity,
    size_t *outWritten);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_TRAINING_TRUST_RECEIPT_H */
