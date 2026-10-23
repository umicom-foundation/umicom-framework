/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_trust_batch_roundtrip.c
 *
 * PURPOSE:
 *   Prove that multiple real signed publications are accepted only after
 *   independent policy checks, and that ordered JSON and SHA-256 identities
 *   repeat exactly for the same reviewed evidence and evaluation instant.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "training_attestation_fixture.h"
#include "umicom/ai/training/training_trust_batch.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CHECK(check) do { if (!(check)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #check); return 1; \
} } while (0)

static int CreateSigned(UmiAttestationFixture *fixture)
{
    return UmiAiTrainingPublicationSaveNewFile(fixture->base) == UMI_STATUS_OK &&
           UmiAiTrainingAttestationSignNewFile(
               fixture->base, fixture->privatePem) == UMI_STATUS_OK;
}

int main(void)
{
    UmiAttestationFixture first, second;
    char firstFingerprint[UMI_SHA256_HEX_CAPACITY];
    char secondFingerprint[UMI_SHA256_HEX_CAPACITY];
    char policyPath[256];
    char policy[1024];
    char json[UMI_AI_TRAINING_TRUST_BATCH_JSON_CAPACITY];
    char repeated[UMI_AI_TRAINING_TRUST_BATCH_JSON_CAPACITY];
    char reversed[UMI_AI_TRAINING_TRUST_BATCH_JSON_CAPACITY];
    char single[UMI_AI_TRAINING_TRUST_BATCH_JSON_CAPACITY];
    char smaller[8] = "UNCHANG";
    size_t written = 0U, repeatedWritten = 0U, otherWritten = 0U;
    size_t sentinel = 4477U;
    const char *paths[2];
    int n;

    CHECK(UmiTestMakeFixture("batch-first", &first));
    CHECK(UmiTestMakeFixture("batch-second", &second));
    CHECK(CreateSigned(&first) && CreateSigned(&second));
    CHECK(UmiAiTrainingAttestationPublicKeyFingerprint(
        first.trustedPem, firstFingerprint) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingAttestationPublicKeyFingerprint(
        second.trustedPem, secondFingerprint) == UMI_STATUS_OK);
    n = snprintf(policyPath, sizeof(policyPath), "%s.trust", first.base);
    CHECK(n > 0 && (size_t)n < sizeof(policyPath));
    n = snprintf(policy, sizeof(policy),
        "UMICOM-AI-TRUST-1\nkey=%s,%s,100,200,active\n"
        "key=%s,%s,100,200,active\n",
        firstFingerprint, first.trustedPem,
        secondFingerprint, second.trustedPem);
    CHECK(n > 0 && (size_t)n < sizeof(policy));
    CHECK(UmiTestWriteBytes(policyPath, policy, (size_t)n));
    paths[0] = first.base;
    paths[1] = second.base;
    CHECK(UmiAiTrainingTrustBatchVerifyJson(
        paths, 2U, policyPath, UINT64_C(150), json,
        sizeof(json), &written) == UMI_STATUS_OK);
    CHECK(written == strlen(json));
    CHECK(strstr(json, "\"schema\":\"umicom.ai.training.trust-batch\"") != NULL);
    CHECK(strstr(json, "\"result\":\"verified\"") != NULL);
    CHECK(strstr(json, "\"publication_count\":2") != NULL);
    CHECK(strstr(json, "\"evaluated_at_unix_seconds\":150") != NULL);
    CHECK(strstr(json, "\"batch_sha256\":\"") != NULL);
    CHECK(strstr(json, "\"receipt_sha256\":\"") != NULL);
    CHECK(strstr(json, "\"index\":0") != NULL);
    CHECK(strstr(json, "\"index\":1") != NULL);
    CHECK(strstr(json, "\"all_publications_verified\":true") != NULL);
    CHECK(strstr(json, "\"batch_record_signed\":false") != NULL);
    CHECK(strstr(json, firstFingerprint) != NULL);
    CHECK(strstr(json, secondFingerprint) != NULL);
    CHECK(strstr(json, first.base) == NULL);
    CHECK(strstr(json, second.base) == NULL);
    CHECK(strstr(json, first.privatePem) == NULL);
    CHECK(strchr(json, '\n') == NULL);

    CHECK(UmiAiTrainingTrustBatchVerifyJson(
        paths, 2U, policyPath, UINT64_C(150), repeated,
        sizeof(repeated), &repeatedWritten) == UMI_STATUS_OK);
    CHECK(written == repeatedWritten);
    CHECK(memcmp(json, repeated, written + 1U) == 0);

    /* Reversing a release manifest must alter the ordered batch identity. */
    paths[0] = second.base;
    paths[1] = first.base;
    CHECK(UmiAiTrainingTrustBatchVerifyJson(
        paths, 2U, policyPath, UINT64_C(150), reversed,
        sizeof(reversed), &otherWritten) == UMI_STATUS_OK);
    CHECK(strstr(reversed, "\"publication_count\":2") != NULL);
    CHECK(strcmp(reversed, json) != 0);
    CHECK(otherWritten > 0U);

    /* A one-entry batch is independently valid, not a false 2/2 success. */
    CHECK(UmiAiTrainingTrustBatchVerifyJson(
        paths, 1U, policyPath, UINT64_C(150), single,
        sizeof(single), &otherWritten) == UMI_STATUS_OK);
    CHECK(strstr(single, "\"publication_count\":1") != NULL);
    CHECK(strstr(single, "\"index\":1") == NULL);

    /* On insufficient capacity both the byte buffer AND length are intact. */
    CHECK(UmiAiTrainingTrustBatchVerifyJson(
        paths, 2U, policyPath, UINT64_C(150), smaller,
        sizeof(smaller), &sentinel) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(smaller, "UNCHANG", sizeof(smaller)) == 0);
    CHECK(sentinel == 4477U);

    CHECK(remove(policyPath) == 0);
    UmiTestDeleteFixture(&first);
    UmiTestDeleteFixture(&second);
    puts("Native signed trust batch: 2/2 ordered and failure-atomic PASS");
    return 0;
}
