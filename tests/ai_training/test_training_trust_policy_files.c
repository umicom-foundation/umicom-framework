/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_trust_policy_files.c
 * PURPOSE: Validate bounded trust-policy file loading and read-only failures.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/training_trust_policy.h"
#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
    return 1; } } while (0)

int main(void)
{
    const char valid[] =
        "UMICOM-AI-TRUST-1\n"
        "key=0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef,keys/author.pem,0,9999999999,active\n";
    const char name[] = "umicom-trust-file-regression.local";
    UmiAiTrainingTrustPolicy result, original;
    FILE *file;
    CHECK(UmiAiTrainingTrustPolicyLoadFile(
          "umicom-trust-policy-file-not-present.local", &result) ==
          UMI_STATUS_IO_ERROR);
    file = fopen(name, "wb");
    CHECK(file != NULL);
    CHECK(fwrite(valid, 1U, sizeof(valid) - 1U, file) == sizeof(valid) - 1U);
    CHECK(fclose(file) == 0);
    CHECK(UmiAiTrainingTrustPolicyLoadFile(name, &result) == UMI_STATUS_OK);
    CHECK(result.keyCount == 1U);
    original = result;
    file = fopen(name, "wb");
    CHECK(file != NULL);
    CHECK(fputs("UMICOM-AI-TRUST-1\nunknown=yes\n", file) >= 0);
    CHECK(fclose(file) == 0);
    CHECK(UmiAiTrainingTrustPolicyLoadFile(name, &result) == UMI_STATUS_PARSE_ERROR);
    CHECK(memcmp(&result, &original, sizeof(result)) == 0);
    file = fopen(name, "wb");
    CHECK(file != NULL);
    for (size_t i = 0U; i < UMI_AI_TRAINING_TRUST_MAX_FILE_BYTES + 2U; ++i)
        CHECK(fputc('x', file) != EOF);
    CHECK(fclose(file) == 0);
    CHECK(UmiAiTrainingTrustPolicyLoadFile(name, &result) ==
          UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&result, &original, sizeof(result)) == 0);
    CHECK(remove(name) == 0);
    puts("Bounded trust policy file reader and failure-atomic output: PASS");
    return 0;
}
