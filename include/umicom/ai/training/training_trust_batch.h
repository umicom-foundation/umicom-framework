/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/training_trust_batch.h
 *
 * PURPOSE:
 *   Verify several signed native training publications as one all-or-nothing
 *   release decision, then produce a deterministic machine-readable receipt.
 *
 * ARCHITECTURE AND TRUST:
 *   Reuse the Framework's existing trust-policy, Ed25519, publication-file
 *   and single-receipt authorities. This service does not implement signature
 *   mathematics, parse model weights, execute training, or contact a server.
 *   The caller supplies an independently approved trust-policy path and one
 *   explicit Unix evaluation time for the entire batch.
 *
 *   This output is UNSIGNED. Its SHA-256 is a reproducible content identifier,
 *   not a signature, transparency log, trustworthy timestamp or proof of host
 *   integrity. Verification reads several files sequentially, NOT as one
 *   atomic filesystem snapshot: protect the source files and check again if
 *   an adversary can replace them while the verifier is running.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_TRAINING_TRUST_BATCH_H
#define UMICOM_AI_TRAINING_TRAINING_TRUST_BATCH_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/training_trust_receipt.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Conservative bounds: one review can inspect at most eight independent
 * publications and cannot allocate or output an unbounded JSON document. */
#define UMI_AI_TRAINING_TRUST_BATCH_MAX_CHECKPOINTS 8U
#define UMI_AI_TRAINING_TRUST_BATCH_JSON_CAPACITY 24576U

/* Verify EVERY publication in the given order. A duplicate verified receipt
 * (even through an alternative pathname) is rejected; ordering is meaningful
 * and included in the batch digest. No untrusted local filenames are echoed.
 *
 * Each entry embeds the exact canonical Framework trust receipt and its own
 * SHA-256. The aggregate SHA-256 commits to a domain separator, evaluation
 * time, entry count and length-delimited receipts in the supplied order.
 *
 * A failure, including insufficient output capacity, leaves outJson bytes
 * and *outWritten completely unchanged. Success writes exactly one compact
 * NUL-terminated JSON object; *outWritten excludes the terminating NUL.
 * No partial-success JSON is ever published by this API.
 *
 * All signatures, source files and operator-controlled key policy MUST still
 * be retained independently. This service does not claim model correctness,
 * training-data rights, a trusted signer clock or a trusted execution host. */
UmiStatus UmiAiTrainingTrustBatchVerifyJson(
    const char *const *checkpointPaths,
    size_t checkpointCount,
    const char *independentlyApprovedPolicyFile,
    uint64_t verificationUnixSeconds,
    char *outJson,
    size_t outputCapacity,
    size_t *outWritten);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_TRAINING_TRUST_BATCH_H */
