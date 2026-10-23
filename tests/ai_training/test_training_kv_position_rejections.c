/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_kv_position_rejections.c
 * PURPOSE:
 *   Rejected input must not overwrite the caller's qualification evidence.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/kv_position_qualification.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expr); \
    return 1; } } while (0)

int main(void)
{
    const UmiAiTrainingDecoderConfig config = {
        16U, 8U, 8U, 12U, 4U, UINT64_C(7172)
    };
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingKvPositionQualification report;
    unsigned char snapshot[sizeof report];
    const size_t tokens[] = {1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U};
    const size_t invalidTokens[] = {1U, 2U, 3U, 4U, 16U, 6U, 7U, 8U};

    CHECK(UmiAiTrainingDecoderCreate(&config, &model) == UMI_STATUS_OK);
    memset(&report, 0xA7, sizeof report);
    memcpy(snapshot, &report, sizeof report);

    CHECK(UmiAiTrainingKvPositionQualify(NULL, tokens, 8U, 9U,
        &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingKvPositionQualify(model, NULL, 8U, 9U,
        &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingKvPositionQualify(model, tokens, 0U, 9U,
        &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingKvPositionQualify(model, tokens, 7U, 9U,
        &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingKvPositionQualify(model, tokens, 9U, 9U,
        &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingKvPositionQualify(model, invalidTokens, 8U, 9U,
        &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingKvPositionQualify(model, tokens, 8U, 16U,
        &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingKvPositionQualify(model, tokens, 8U, 9U,
        NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(snapshot, &report, sizeof report) == 0);

    UmiAiTrainingDecoderDestroy(model);
    puts("KV position qualification: input rejections and report isolation PASS");
    return 0;
}
