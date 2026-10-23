/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_trust_receipt_rejections.c
 *
 * PURPOSE:
 *   A refused publisher, tampered bundle or invalid output MUST NOT emit any
 *   machine-readable success receipt or mutate an earlier output buffer.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "training_attestation_fixture.h"
#include "umicom/ai/training/training_trust_receipt.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CHECK(value) do { if (!(value)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); \
    return 1; } } while (0)

static int AssertRejectedWithoutPublication(const char *checkpoint,
                                            const char *policy,
                                            uint64_t seconds)
{
    char receipt[UMI_AI_TRAINING_TRUST_RECEIPT_JSON_CAPACITY];
    char preserved[UMI_AI_TRAINING_TRUST_RECEIPT_JSON_CAPACITY];
    size_t written = 999U;
    memset(receipt, 0xA5, sizeof(receipt));
    memcpy(preserved, receipt, sizeof(receipt));
    CHECK(UmiAiTrainingTrustReceiptVerifyJson(
          checkpoint, policy, seconds, receipt,
          sizeof(receipt), &written) != UMI_STATUS_OK);
    CHECK(written == 999U);
    CHECK(memcmp(receipt, preserved, sizeof(receipt)) == 0);
    return 0;
}

static int WritePolicy(const char *policyPath, const char *key,
                       const char *pemPath, const char *state)
{
    char policy[512];
    int n = snprintf(policy, sizeof(policy),
        "UMICOM-AI-TRUST-1\nkey=%s,%s,100,200,%s\n",
        key, pemPath, state);
    return n > 0 && (size_t)n < sizeof(policy) &&
           UmiTestWriteBytes(policyPath, policy, (size_t)n);
}

int main(void)
{
    UmiAttestationFixture fixture;
    char fingerprint[UMI_SHA256_HEX_CAPACITY];
    char wrongKey[UMI_SHA256_HEX_CAPACITY];
    char policyPath[256];
    char original[UMI_AI_TRAINING_TRUST_RECEIPT_JSON_CAPACITY];
    size_t length = 0U, sentinel = 881U;
    int n;

    CHECK(UmiTestMakeFixture("receipt-rejections", &fixture));
    CHECK(UmiAiTrainingAttestationPublicKeyFingerprint(
          fixture.trustedPem, fingerprint) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingAttestationPublicKeyFingerprint(
          fixture.wrongPem, wrongKey) == UMI_STATUS_OK);
    n = snprintf(policyPath, sizeof(policyPath), "%s.trust", fixture.base);
    CHECK(n > 0 && (size_t)n < sizeof(policyPath));
    CHECK(WritePolicy(policyPath, fingerprint, fixture.trustedPem, "active"));
    CHECK(UmiAiTrainingPublicationSaveNewFile(fixture.base) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingAttestationSignNewFile(
          fixture.base, fixture.privatePem) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTrustReceiptVerifyJson(
          fixture.base, policyPath, UINT64_C(150), original,
          sizeof(original), &length) == UMI_STATUS_OK);
    CHECK(length > 0U);

    CHECK(AssertRejectedWithoutPublication(fixture.base, policyPath, 99U) == 0);
    CHECK(AssertRejectedWithoutPublication(fixture.base, policyPath, 200U) == 0);
    CHECK(WritePolicy(policyPath, fingerprint, fixture.trustedPem, "revoked"));
    CHECK(AssertRejectedWithoutPublication(fixture.base, policyPath, 150U) == 0);
    CHECK(WritePolicy(policyPath, wrongKey, fixture.wrongPem, "active"));
    CHECK(AssertRejectedWithoutPublication(fixture.base, policyPath, 150U) == 0);
    CHECK(WritePolicy(policyPath, fingerprint, fixture.wrongPem, "active"));
    CHECK(AssertRejectedWithoutPublication(fixture.base, policyPath, 150U) == 0);
    CHECK(WritePolicy(policyPath, fingerprint, fixture.trustedPem, "active"));
    CHECK(AssertRejectedWithoutPublication(
          fixture.base, "missing-approved-trust-policy", 150U) == 0);

    /* Invalid output storage is refused before attempting verification. */
    CHECK(UmiAiTrainingTrustReceiptVerifyJson(
          fixture.base, policyPath, 150U, NULL, 10U,
          &sentinel) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(sentinel == 881U);
    CHECK(UmiAiTrainingTrustReceiptVerifyJson(
          fixture.base, policyPath, 150U, original, sizeof(original),
          NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingTrustReceiptVerifyJson(
          NULL, policyPath, 150U, original, sizeof(original),
          &sentinel) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(sentinel == 881U);

    /* The signature is still valid, but a changed underlying source-lock
     * file must force rejection before machine-readable success. */
    CHECK(UmiTestWriteBytes(fixture.source, "changed source snapshot\n", 24U));
    CHECK(AssertRejectedWithoutPublication(fixture.base, policyPath, 150U) == 0);
    CHECK(remove(policyPath) == 0);
    UmiTestDeleteFixture(&fixture);
    puts("Native C23 signed trust JSON receipt fail-closed rejection: PASS");
    return 0;
}
