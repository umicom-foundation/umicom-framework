/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_trust_batch_rejections.c
 *
 * PURPOSE:
 *   Reject duplicate, expired and tampered release-batch items without
 *   publishing a misleading partial JSON success result.
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

static int RejectedUnchanged(const char *const *paths, size_t count,
                            const char *policy, uint64_t second,
                            UmiStatus expected)
{
    char json[UMI_AI_TRAINING_TRUST_BATCH_JSON_CAPACITY];
    size_t written = 0xA5A5U;
    memset(json, 0x6DU, sizeof(json));
    CHECK(UmiAiTrainingTrustBatchVerifyJson(paths, count, policy, second,
          json, sizeof(json), &written) == expected);
    CHECK(written == 0xA5A5U);
    for (size_t index = 0U; index < sizeof(json); ++index)
        CHECK((unsigned char)json[index] == 0x6DU);
    return 0;
}

int main(void)
{
    UmiAttestationFixture first, second;
    char fingerprintFirst[UMI_SHA256_HEX_CAPACITY];
    char fingerprintSecond[UMI_SHA256_HEX_CAPACITY];
    char policyPath[256];
    char policy[1024];
    char destination[UMI_AI_TRAINING_TRUST_BATCH_JSON_CAPACITY];
    size_t written = 0U;
    const char *paths[UMI_AI_TRAINING_TRUST_BATCH_MAX_CHECKPOINTS + 1U];
    int n;

    CHECK(UmiTestMakeFixture("batch-reject-first", &first));
    CHECK(UmiTestMakeFixture("batch-reject-second", &second));
    CHECK(UmiAiTrainingPublicationSaveNewFile(first.base) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingPublicationSaveNewFile(second.base) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingAttestationSignNewFile(first.base, first.privatePem) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingAttestationSignNewFile(second.base, second.privatePem) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingAttestationPublicKeyFingerprint(
        first.trustedPem, fingerprintFirst) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingAttestationPublicKeyFingerprint(
        second.trustedPem, fingerprintSecond) == UMI_STATUS_OK);
    n = snprintf(policyPath, sizeof(policyPath), "%s.trust", first.base);
    CHECK(n > 0 && (size_t)n < sizeof(policyPath));
    n = snprintf(policy, sizeof(policy),
        "UMICOM-AI-TRUST-1\nkey=%s,%s,100,200,active\n"
        "key=%s,%s,100,200,active\n",
        fingerprintFirst, first.trustedPem,
        fingerprintSecond, second.trustedPem);
    CHECK(n > 0 && (size_t)n < sizeof(policy));
    CHECK(UmiTestWriteBytes(policyPath, policy, (size_t)n));
    paths[0] = first.base;
    paths[1] = second.base;
    CHECK(UmiAiTrainingTrustBatchVerifyJson(
        paths, 2U, policyPath, 150U, destination,
        sizeof(destination), &written) == UMI_STATUS_OK);

    CHECK(RejectedUnchanged(paths, 2U, policyPath, 200U,
                             UMI_STATUS_PERMISSION_DENIED) == 0);
    CHECK(RejectedUnchanged(paths, 2U, policyPath, 99U,
                             UMI_STATUS_PERMISSION_DENIED) == 0);
    CHECK(RejectedUnchanged(paths, 0U, policyPath, 150U,
                             UMI_STATUS_INVALID_ARGUMENT) == 0);
    CHECK(RejectedUnchanged(NULL, 2U, policyPath, 150U,
                             UMI_STATUS_INVALID_ARGUMENT) == 0);
    CHECK(RejectedUnchanged(paths, 2U, NULL, 150U,
                             UMI_STATUS_INVALID_ARGUMENT) == 0);
    paths[1] = first.base;
    CHECK(RejectedUnchanged(paths, 2U, policyPath, 150U,
                             UMI_STATUS_ALREADY_EXISTS) == 0);
    paths[1] = NULL;
    CHECK(RejectedUnchanged(paths, 2U, policyPath, 150U,
                             UMI_STATUS_INVALID_ARGUMENT) == 0);
    paths[1] = second.base;
    for (size_t index = 2U; index <= UMI_AI_TRAINING_TRUST_BATCH_MAX_CHECKPOINTS;
         ++index) paths[index] = first.base;
    CHECK(RejectedUnchanged(paths, UMI_AI_TRAINING_TRUST_BATCH_MAX_CHECKPOINTS + 1U,
                             policyPath, 150U, UMI_STATUS_CAPACITY_EXCEEDED) == 0);

    /* One bad publication must block the entire batch, not report 1/2 as
     * "verified". Mutate the second signed checkpoint, not the first. */
    {
        FILE *stream = fopen(second.clip, "ab");
        CHECK(stream != NULL);
        CHECK(fwrite("tampered", 1U, 8U, stream) == 8U);
        CHECK(fclose(stream) == 0);
    }
    paths[1] = second.base;
    CHECK(UmiAiTrainingTrustBatchVerifyJson(
        paths, 1U, policyPath, 150U, destination,
        sizeof(destination), &written) == UMI_STATUS_OK);
    /* The second checkpoint, not the trusted first, is now invalid. The
     * canonical verifier's status may distinguish file corruption from a
     * signature error, so assert only that it is rejected. */
    {
        char output[UMI_AI_TRAINING_TRUST_BATCH_JSON_CAPACITY];
        size_t count = 123456U;
        memset(output, 0xA0, sizeof(output));
        CHECK(UmiAiTrainingTrustBatchVerifyJson(
            paths, 2U, policyPath, 150U, output,
            sizeof(output), &count) != UMI_STATUS_OK);
        CHECK(count == 123456U);
        for (size_t i = 0U; i < sizeof(output); ++i)
            CHECK((unsigned char)output[i] == 0xA0U);
    }
    CHECK(remove(policyPath) == 0);
    UmiTestDeleteFixture(&first);
    UmiTestDeleteFixture(&second);
    puts("Native signed trust batch: rejection and atomic publication PASS");
    return 0;
}
