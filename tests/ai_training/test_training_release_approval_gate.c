/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_release_approval_gate.c
 * PURPOSE:
 *   Real Ed25519 two-checkpoint release approval; reject reordering,
 *   changed approval, changed trust and post-signing file mutation.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "training_attestation_fixture.h"
#include "umicom/ai/training/training_release_approval.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CHECK(value) do { if (!(value)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); return 1; \
} } while (0)

static int RejectUnchanged(const char *const *paths, size_t count,
                           const char *policy,
                           const UmiAiTrainingReleaseApproval *approval)
{
    char output[UMI_AI_TRAINING_RELEASE_APPROVAL_JSON_CAPACITY];
    size_t written = 8765U;
    memset(output, 0x61, sizeof(output));
    CHECK(UmiAiTrainingReleaseApprovalVerifyJson(
        paths, count, policy, approval, output, sizeof(output),
        &written) != UMI_STATUS_OK);
    CHECK(written == 8765U);
    for (size_t i = 0U; i < sizeof(output); ++i)
        CHECK(output[i] == 'a');
    return 0;
}

int main(void)
{
    UmiAttestationFixture first, second;
    char fingerprintOne[UMI_SHA256_HEX_CAPACITY];
    char fingerprintTwo[UMI_SHA256_HEX_CAPACITY];
    char policyPath[256];
    char policy[1024];
    char approvedText[256];
    char rawBatch[UMI_AI_TRAINING_TRUST_BATCH_JSON_CAPACITY];
    char approvedJson[UMI_AI_TRAINING_RELEASE_APPROVAL_JSON_CAPACITY];
    char rerun[UMI_AI_TRAINING_RELEASE_APPROVAL_JSON_CAPACITY];
    char small[4] = {'K','E','E','P'};
    UmiAiTrainingReleaseApproval approval;
    UmiAiTrainingReleaseApproval incorrect;
    const char *paths[2];
    const char *needle;
    size_t rawLength = 0U, written = 0U, rerunLength = 0U;
    size_t sentinel = 987U;
    int n;

    CHECK(UmiTestMakeFixture("approved-first", &first));
    CHECK(UmiTestMakeFixture("approved-second", &second));
    CHECK(UmiAiTrainingPublicationSaveNewFile(first.base) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingPublicationSaveNewFile(second.base) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingAttestationSignNewFile(first.base, first.privatePem) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingAttestationSignNewFile(second.base, second.privatePem) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingAttestationPublicKeyFingerprint(
        first.trustedPem, fingerprintOne) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingAttestationPublicKeyFingerprint(
        second.trustedPem, fingerprintTwo) == UMI_STATUS_OK);
    n = snprintf(policyPath, sizeof(policyPath), "%s.trust", first.base);
    CHECK(n > 0 && (size_t)n < sizeof(policyPath));
    n = snprintf(policy, sizeof(policy),
        "UMICOM-AI-TRUST-1\nkey=%s,%s,100,200,active\n"
        "key=%s,%s,100,200,active\n",
        fingerprintOne, first.trustedPem, fingerprintTwo, second.trustedPem);
    CHECK(n > 0 && (size_t)n < sizeof(policy));
    CHECK(UmiTestWriteBytes(policyPath, policy, (size_t)n));

    paths[0] = first.base;
    paths[1] = second.base;
    CHECK(UmiAiTrainingTrustBatchVerifyJson(
        paths, 2U, policyPath, UINT64_C(150), rawBatch,
        sizeof(rawBatch), &rawLength) == UMI_STATUS_OK);
    CHECK(rawLength == strlen(rawBatch));
    needle = strstr(rawBatch, "\"batch_sha256\":\"");
    CHECK(needle != NULL);
    needle += sizeof("\"batch_sha256\":\"") - 1U;
    CHECK(strlen(needle) >= 65U && needle[64] == '"');
    n = snprintf(approvedText, sizeof(approvedText),
        "UMICOM-AI-RELEASE-APPROVAL-1\n"
        "publication_count=2\nevaluated_at_unix_seconds=150\n"
        "batch_sha256=%.*s\n", 64, needle);
    CHECK(n > 0 && (size_t)n < sizeof(approvedText));
    CHECK(UmiAiTrainingReleaseApprovalParse(
        approvedText, strlen(approvedText), &approval) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingReleaseApprovalVerifyJson(
        paths, 2U, policyPath, &approval, approvedJson,
        sizeof(approvedJson), &written) == UMI_STATUS_OK);
    CHECK(written == strlen(approvedJson));
    CHECK(strstr(approvedJson, "\"release_approval\":{") != NULL);
    CHECK(strstr(approvedJson, "\"exact_ordered_batch_pinned\":true") != NULL);
    CHECK(strstr(approvedJson, "\"approval_record_signed\":false") != NULL);
    CHECK(strstr(approvedJson, first.base) == NULL);
    CHECK(strstr(approvedJson, second.base) == NULL);
    CHECK(UmiAiTrainingReleaseApprovalVerifyJson(
        paths, 2U, policyPath, &approval, rerun,
        sizeof(rerun), &rerunLength) == UMI_STATUS_OK);
    CHECK(written == rerunLength && memcmp(approvedJson, rerun, written+1U) == 0);

    /* Full validation finishes before the first caller-owned byte changes. */
    CHECK(UmiAiTrainingReleaseApprovalVerifyJson(
        paths, 2U, policyPath, &approval, small, sizeof(small),
        &sentinel) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(sentinel == 987U && memcmp(small, "KEEP", 4U) == 0);

    incorrect = approval;
    incorrect.batchSha256[3] =
        incorrect.batchSha256[3] == '0' ? '1' : '0';
    CHECK(RejectUnchanged(paths, 2U, policyPath, &incorrect) == 0);
    incorrect = approval;
    incorrect.publicationCount = 1U;
    CHECK(RejectUnchanged(paths, 2U, policyPath, &incorrect) == 0);
    incorrect = approval;
    incorrect.evaluatedAtUnixSeconds = 151U;
    CHECK(RejectUnchanged(paths, 2U, policyPath, &incorrect) == 0);

    paths[0] = second.base;
    paths[1] = first.base;
    CHECK(RejectUnchanged(paths, 2U, policyPath, &approval) == 0);
    paths[0] = first.base;
    paths[1] = second.base;

    /* Changing the independently approved key state immediately revokes
     * approval even if the pinned checkpoint bytes have not changed. */
    n = snprintf(policy, sizeof(policy),
        "UMICOM-AI-TRUST-1\nkey=%s,%s,100,200,active\n"
        "key=%s,%s,100,200,revoked\n",
        fingerprintOne, first.trustedPem, fingerprintTwo, second.trustedPem);
    CHECK(n > 0 && (size_t)n < sizeof(policy));
    CHECK(UmiTestWriteBytes(policyPath, policy, (size_t)n));
    CHECK(RejectUnchanged(paths, 2U, policyPath, &approval) == 0);

    /* Repair local test policy, then mutate one file AFTER signing. */
    n = snprintf(policy, sizeof(policy),
        "UMICOM-AI-TRUST-1\nkey=%s,%s,100,200,active\n"
        "key=%s,%s,100,200,active\n",
        fingerprintOne, first.trustedPem, fingerprintTwo, second.trustedPem);
    CHECK(n > 0 && (size_t)n < sizeof(policy));
    CHECK(UmiTestWriteBytes(policyPath, policy, (size_t)n));
    CHECK(UmiTestWriteBytes(second.source, "modified input evidence", 23U));
    CHECK(RejectUnchanged(paths, 2U, policyPath, &approval) == 0);

    CHECK(remove(policyPath) == 0);
    UmiTestDeleteFixture(&first);
    UmiTestDeleteFixture(&second);
    puts("Exact ordered release approval and revoked/tampered rejection PASS");
    return 0;
}
