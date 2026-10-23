/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_gradient_policy_identity.c
 * PURPOSE: Verify digest binding, binary32 threshold identity, batch policy,
 *          exact record grammar and atomic publication of output buffers.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------
 * LICENCE: MIT
 */
#include "umicom/ai/training/gradient_policy.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
    return 1; } } while (0)

int main(void)
{
    const uint8_t data[] = {0x11U, 0x29U, 0x80U, 0xC0U, 0xFFU};
    uint8_t modified[sizeof(data)];
    UmiAiTrainingGradientPolicy policy = {1.0F, 4U, 2U};
    UmiAiTrainingGradientPolicy different;
    char record[UMI_AI_GRADIENT_POLICY_RECORD_CAPACITY];
    char sentinel[2] = {'X', '\0'};
    size_t written = 123U;
    CHECK(UmiAiTrainingGradientPolicyFormat(
        data, sizeof(data), &policy, record, sizeof(record), &written) == UMI_STATUS_OK);
    CHECK(written == strlen(record) && strstr(record, "maximum_gradient_norm_bits=3f800000\n") != NULL);
    CHECK(strstr(record, "batch_sequences=4\nmicrobatch_sequences=2\n") != NULL);
    CHECK(UmiAiTrainingGradientPolicyVerify(
        data, sizeof(data), &policy, record, written) == UMI_STATUS_OK);
    memcpy(modified, data, sizeof(data));
    modified[2] ^= 0x01U;
    CHECK(UmiAiTrainingGradientPolicyVerify(
        modified, sizeof(modified), &policy, record, written) == UMI_STATUS_PARSE_ERROR);
    different = policy;
    different.maximumGradientNorm = 1.000001F;
    CHECK(UmiAiTrainingGradientPolicyVerify(
        data, sizeof(data), &different, record, written) == UMI_STATUS_PARSE_ERROR);
    different = policy;
    different.microbatchSequences = 1U;
    CHECK(UmiAiTrainingGradientPolicyVerify(
        data, sizeof(data), &different, record, written) == UMI_STATUS_PARSE_ERROR);
    different = policy;
    different.batchSequences = 8U;
    CHECK(UmiAiTrainingGradientPolicyVerify(
        data, sizeof(data), &different, record, written) == UMI_STATUS_PARSE_ERROR);
    {
        char changed[sizeof(record)];
        memcpy(changed, record, written + 1U);
        changed[written - 2U] ^= 1;
        CHECK(UmiAiTrainingGradientPolicyVerify(
            data, sizeof(data), &policy, changed, written) == UMI_STATUS_PARSE_ERROR);
        CHECK(UmiAiTrainingGradientPolicyVerify(
            data, sizeof(data), &policy, record, written + 1U) == UMI_STATUS_PARSE_ERROR);
    }
    CHECK(UmiAiTrainingGradientPolicyFormat(
        data, sizeof(data), &policy, sentinel, sizeof(sentinel),
        &written) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(sentinel[0] == 'X' && written != 0U);
    different = policy;
    different.maximumGradientNorm = 0.0F;
    different.microbatchSequences = 0U;
    CHECK(UmiAiTrainingGradientPolicyFormat(
        data, sizeof(data), &different, record, sizeof(record), &written) == UMI_STATUS_OK);
    CHECK(strstr(record, "maximum_gradient_norm_bits=00000000") != NULL);
    CHECK(UmiAiTrainingGradientPolicyVerify(
        data, sizeof(data), &different, record, written) == UMI_STATUS_OK);
    different.maximumGradientNorm = -1.0F;
    CHECK(UmiAiTrainingGradientPolicyVerify(
        data, sizeof(data), &different, record, written) == UMI_STATUS_INVALID_ARGUMENT);
    puts("clipping policy record integrity and failure atomicity: PASS");
    return 0;
}
