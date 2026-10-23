/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_trust_receipt_roundtrip.c
 *
 * PURPOSE:
 *   Exercise the real publication, Ed25519, independently approved publisher
 *   policy and failure-atomic machine-readable JSON receipt as ONE workflow.
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

int main(void)
{
    UmiAttestationFixture fixture;
    UmiAiTrainingPublicationEvidence inspected;
    char keyHex[UMI_SHA256_HEX_CAPACITY];
    char policyPath[256];
    char policy[512];
    char first[UMI_AI_TRAINING_TRUST_RECEIPT_JSON_CAPACITY];
    char repeated[UMI_AI_TRAINING_TRUST_RECEIPT_JSON_CAPACITY];
    char tiny[8] = "PRESERV";
    char exact[UMI_AI_TRAINING_TRUST_RECEIPT_JSON_CAPACITY];
    size_t firstLength = 0U, repeatedLength = 0U, sentinel = 777U;
    int n;

    CHECK(UmiTestMakeFixture("receipt-roundtrip", &fixture));
    CHECK(UmiAiTrainingAttestationPublicKeyFingerprint(
          fixture.trustedPem, keyHex) == UMI_STATUS_OK);
    n = snprintf(policyPath, sizeof(policyPath), "%s.trust", fixture.base);
    CHECK(n > 0 && (size_t)n < sizeof(policyPath));
    n = snprintf(policy, sizeof(policy),
          "UMICOM-AI-TRUST-1\nkey=%s,%s,100,200,active\n",
          keyHex, fixture.trustedPem);
    CHECK(n > 0 && (size_t)n < sizeof(policy));
    CHECK(UmiTestWriteBytes(policyPath, policy, (size_t)n));
    CHECK(UmiAiTrainingPublicationSaveNewFile(fixture.base) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingAttestationSignNewFile(
          fixture.base, fixture.privatePem) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingPublicationVerifyAndInspectFile(
          fixture.base, &inspected) == UMI_STATUS_OK);

    memset(first, 0x47, sizeof(first));
    CHECK(UmiAiTrainingTrustReceiptVerifyJson(
          fixture.base, policyPath, UINT64_C(150), first,
          sizeof(first), &firstLength) == UMI_STATUS_OK);
    CHECK(firstLength == strlen(first));
    CHECK(firstLength > 600U && firstLength < sizeof(first));
    CHECK(strstr(first, "\"schema\":\"umicom.ai.training.trust-receipt\"") != NULL);
    CHECK(strstr(first, "\"result\":\"verified\"") != NULL);
    CHECK(strstr(first, "\"evaluated_at_unix_seconds\":150") != NULL);
    CHECK(strstr(first, keyHex) != NULL);
    CHECK(strstr(first, inspected.checkpoint.sha256) != NULL);
    CHECK(strstr(first, inspected.clipPolicy.sha256) != NULL);
    CHECK(strstr(first, inspected.trainingLock.sha256) != NULL);
    CHECK(strstr(first, inspected.sourceLock.sha256) != NULL);
    CHECK(strstr(first, "\"ed25519_signature\":true") != NULL);
    CHECK(strstr(first, "\"trusted_signing_timestamp\":false") != NULL);
    CHECK(strstr(first, "\"host_integrity_attested\":false") != NULL);
    CHECK(strstr(first, "\"dataset_rights_reviewed\":false") != NULL);
    CHECK(strstr(first, fixture.base) == NULL);
    CHECK(strstr(first, fixture.privatePem) == NULL);
    CHECK(strstr(first, fixture.trustedPem) == NULL);
    CHECK(strchr(first, '\n') == NULL);

    CHECK(UmiAiTrainingTrustReceiptVerifyJson(
          fixture.base, policyPath, UINT64_C(150), repeated,
          sizeof(repeated), &repeatedLength) == UMI_STATUS_OK);
    CHECK(repeatedLength == firstLength);
    CHECK(memcmp(first, repeated, firstLength + 1U) == 0);

    CHECK(UmiAiTrainingTrustReceiptVerifyJson(
          fixture.base, policyPath, UINT64_C(150), tiny,
          sizeof(tiny), &sentinel) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(tiny, "PRESERV", sizeof(tiny)) == 0);
    CHECK(sentinel == 777U);

    /* Exactly the required buffer length including NUL is sufficient;
     * space requirements are in bytes, not characters or file records. */
    CHECK(UmiAiTrainingTrustReceiptVerifyJson(
          fixture.base, policyPath, UINT64_C(150), exact,
          firstLength + 1U, &repeatedLength) == UMI_STATUS_OK);
    CHECK(repeatedLength == firstLength);
    CHECK(memcmp(first, exact, firstLength + 1U) == 0);

    CHECK(remove(policyPath) == 0);
    UmiTestDeleteFixture(&fixture);
    puts("Native C23 signed trust JSON receipt roundtrip: PASS");
    return 0;
}
