/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/training_approval_attestation.h
 *
 * PURPOSE:
 *   Independently sign a reviewed native LLM release-approval record and
 *   require an explicit external Ed25519 approval key before release gating.
 *
 * SECURITY AND OWNERSHIP:
 *   This optional C23 interface depends on OpenSSL 3 through the existing
 *   Framework attestation component. The publisher's trust policy and the
 *   release-approver's public key are separate, caller-controlled inputs.
 *   A signature is not proof of legal authority, a trusted signing time, a
 *   secure host, or model accuracy. Filesystem paths are not a sandbox.
 *   All output buffers remain unchanged when a call fails.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_TRAINING_APPROVAL_ATTESTATION_H
#define UMICOM_AI_TRAINING_TRAINING_APPROVAL_ATTESTATION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/training_release_approval.h"
#include "umicom/base/sha256.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_TRAINING_APPROVAL_SIGNATURE_SUFFIX ".ed25519"
#define UMI_AI_TRAINING_SIGNED_APPROVAL_JSON_CAPACITY \
    (UMI_AI_TRAINING_RELEASE_APPROVAL_JSON_CAPACITY + 256U)

/* Sign the EXACT on-disk approval bytes with the domain-separated message:
 * "UMICOM-AI-RELEASE-APPROVAL-ED25519-1\\n" || original_file_bytes.
 * The text must first parse under the canonical approval-plan parser.
 * The caller's existing approval file is unchanged. Publish a NEW companion
 * <approvalPath>.ed25519 using exclusive staging and no-clobber semantics.
 * Never store a private key in the source tree or distributable ZIP. */
UmiStatus UmiAiTrainingApprovalAttestationSignNewFile(
    const char *approvalPath, const char *privateEd25519PemPath);

/* Check canonical companion grammar, raw-public-key SHA-256 and Ed25519
 * signature over the exact approval bytes. The public key MUST be supplied
 * by an independently approved authority, never selected from the detached
 * signature itself. Both output structs/buffers remain untouched on failure.
 * Either output pointer may not be NULL. */
UmiStatus UmiAiTrainingApprovalAttestationVerifyFile(
    const char *approvalPath, const char *trustedApprovalPublicKeyPemPath,
    UmiAiTrainingReleaseApproval *outApproval,
    char outSignerSha256[UMI_SHA256_HEX_CAPACITY]);

/* Verify the approval signature first; then re-run the existing Framework
 * publisher-policy and ordered-checkpoint release gate. On success emit the
 * original verified batch fields plus an explicit signed-approval marker and
 * the independently supplied approval key identity. The original unsigned
 * UmiAiTrainingReleaseApprovalVerifyJson API remains unchanged. */
UmiStatus UmiAiTrainingSignedReleaseApprovalVerifyJson(
    const char *const *checkpointPaths,
    size_t checkpointCount,
    const char *trustedPublisherPolicyFile,
    const char *approvalPath,
    const char *trustedApprovalPublicKeyPemPath,
    char *outJson,
    size_t outputCapacity,
    size_t *outWritten);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_TRAINING_APPROVAL_ATTESTATION_H */
