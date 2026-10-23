/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/training_release_approval.h
 *
 * PURPOSE:
 *   Require an independently approved, exact ordered release-batch identity
 *   before accepting otherwise valid signed LLM checkpoint publications.
 *
 * ARCHITECTURE AND TRUST:
 *   Reuse the canonical Framework trust-batch verifier, offline publisher
 *   policy, Ed25519 verifier and SHA-256 service. The approval record is a
 *   reviewed local input stored separately from untrusted release artefacts;
 *   it is NOT automatically signed or authenticated by this API. A digest
 *   comparison cannot replace secure approval-file distribution, publisher
 *   revocation, host review, model evaluation or a trusted signing timestamp.
 *   Checkpoint files are examined sequentially, not as an atomic snapshot.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_TRAINING_RELEASE_APPROVAL_H
#define UMICOM_AI_TRAINING_TRAINING_RELEASE_APPROVAL_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/training_trust_batch.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_TRAINING_RELEASE_APPROVAL_MAX_BYTES 512U
#define UMI_AI_TRAINING_RELEASE_APPROVAL_JSON_CAPACITY \
    (UMI_AI_TRAINING_TRUST_BATCH_JSON_CAPACITY + 192U)

/* Approved text (LF or CRLF; comments start with '#' in column one):
 *
 *   UMICOM-AI-RELEASE-APPROVAL-1
 *   publication_count=2
 *   evaluated_at_unix_seconds=150
 *   batch_sha256=<64 lower-case hexadecimal digits>
 *
 * The three keys are mandatory and may occur once each in any order after
 * the header. Unknown/duplicate keys, whitespace, NUL and trailing data are
 * rejected. This format deliberately contains no paths, embedded scripts,
 * environment substitutions, public keys or network addresses.
 *
 * The evaluation instant must be reviewed independently. It is not a trusted
 * time source: previously valid intervals can be revisited unless the
 * operator's externally managed trust/revocation policy forbids that. */
typedef struct UmiAiTrainingReleaseApproval {
    uint32_t publicationCount;
    uint64_t evaluatedAtUnixSeconds;
    char batchSha256[65];
} UmiAiTrainingReleaseApproval;

/* Strict, bounded parser and local file loader. Both preserve *outApproval
 * byte for byte if validation, read or capacity checks fail. Only local
 * caller-approved text is read; neither function accesses checkpoint files. */
UmiStatus UmiAiTrainingReleaseApprovalParse(
    const char *text, size_t textLength,
    UmiAiTrainingReleaseApproval *outApproval);
UmiStatus UmiAiTrainingReleaseApprovalLoadFile(
    const char *approvalPath,
    UmiAiTrainingReleaseApproval *outApproval);

/* Re-verify every signed publication under the independently approved
 * publisher trust policy at the approval's explicitly recorded instant.
 * The order, count and computed SHA-256 must match the approved record.
 *
 * On success the canonical verified batch JSON is extended with a clear,
 * unsigned "release_approval" field. A rejected entry, reordered checkpoint,
 * different selected time or insufficient output capacity leaves BOTH the
 * caller's JSON buffer and written-length output entirely untouched.
 * Paths and approval contents are not echoed in JSON. */
UmiStatus UmiAiTrainingReleaseApprovalVerifyJson(
    const char *const *checkpointPaths,
    size_t checkpointCount,
    const char *independentlyApprovedPolicyFile,
    const UmiAiTrainingReleaseApproval *approval,
    char *outJson,
    size_t outputCapacity,
    size_t *outWritten);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_TRAINING_RELEASE_APPROVAL_H */
