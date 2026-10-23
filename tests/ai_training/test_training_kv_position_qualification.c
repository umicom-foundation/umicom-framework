/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_kv_position_qualification.c
 * PURPOSE:
 *   Qualify canonical decoder logits against real KV Prefix/Append and the
 *   first learned-absolute-position rollover. Preserve checkpoint bytes.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/kv_position_qualification.h"
#include "umicom/ai/training/checkpoint.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expr); \
    return 1; } } while (0)

int main(void)
{
    const UmiAiTrainingDecoderConfig cases[] = {
        {5U, 1U, 4U, 8U, 1U, UINT64_C(101)},
        {16U, 8U, 8U, 12U, 4U, UINT64_C(202)},
        {260U, 8U, 8U, 16U, 2U, UINT64_C(303)},
        {32U, 16U, 16U, 32U, 8U, UINT64_C(404)}
    };
    for (size_t caseId = 0U; caseId < sizeof(cases) / sizeof(cases[0]); ++caseId) {
        const UmiAiTrainingDecoderConfig *config = &cases[caseId];
        UmiAiTrainingDecoder *model = NULL;
        UmiAiTrainingKvPositionQualification report = {0};
        uint8_t *before = NULL, *after = NULL;
        size_t bytes = 0U, written = 0U;
        size_t tokens[UMI_AI_DECODER_MAX_CONTEXT];
        const size_t count = (size_t)config->contextLength;
        const size_t vocabulary = (size_t)config->vocabularySize;
        const size_t nextToken = (count * 17U + 2U) % vocabulary;

        CHECK(UmiAiTrainingDecoderCreate(config, &model) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingDecoderCheckpointSize(model, &bytes) == UMI_STATUS_OK);
        before = malloc(bytes);
        after = malloc(bytes);
        CHECK(before != NULL && after != NULL);
        CHECK(UmiAiTrainingDecoderCheckpointEncode(model, before,
            bytes, &written) == UMI_STATUS_OK && written == bytes);

        for (size_t index = 0U; index < count; ++index)
            tokens[index] = (index * 13U + 3U) % vocabulary;
        CHECK(UmiAiTrainingKvPositionQualify(model,
            tokens, count, nextToken, &report) == UMI_STATUS_OK);
        CHECK(report.contextTokens == count);
        CHECK(report.vocabularySize == vocabulary);
        CHECK(report.comparedLogits == 2U * vocabulary);
        CHECK(report.cacheHeapBytes > 0U);
        CHECK(report.computedRowsBeforeRollover == count);
        CHECK(report.computedRowsAfterRollover == count * 2U);
        CHECK(report.fullWindowRebuilds == 1U);
        CHECK(report.maximumAbsoluteDifferenceBeforeRollover >= 0.0F);
        CHECK(report.maximumAbsoluteDifferenceBeforeRollover <= 0.00004F);
        CHECK(report.maximumAbsoluteDifferenceAfterRollover >= 0.0F);
        CHECK(report.maximumAbsoluteDifferenceAfterRollover <= 0.00004F);

        CHECK(UmiAiTrainingDecoderCheckpointEncode(model, after,
            bytes, &written) == UMI_STATUS_OK && written == bytes);
        CHECK(memcmp(before, after, bytes) == 0);
        free(after);
        free(before);
        UmiAiTrainingDecoderDestroy(model);
    }
    puts("KV position qualification: canonical decoder logits and rollover parity PASS");
    return 0;
}
