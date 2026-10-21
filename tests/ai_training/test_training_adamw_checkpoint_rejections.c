/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_adamw_checkpoint_rejections.c
 * PURPOSE: Reject tampered/truncated checkpoint envelopes, stale optimizer
 *          bindings and corrupted hyperparameters without publishing handles.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/optimizer_checkpoint.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #expr); return 1; \
} } while (0)

int main(void)
{
    UmiAiTrainingDecoderConfig cfg = {12U, 3U, 4U, 8U, 2U, 119U};
    UmiAiTrainingDecoder *model = NULL, *other = NULL, *untrusted = NULL;
    UmiAiTrainingAdamW *adam = NULL, *untrustedAdam = NULL;
    UmiAiTrainingAdamWConfig config;
    size_t count = 0U, used = 99U;
    uint8_t *bytes = NULL;
    CHECK(UmiAiTrainingAdamWDefaults(&config) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCreate(&cfg, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCreate(&cfg, &other) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderAdamWCreate(model, &config, &adam) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingAdamWCheckpointSize(other, adam, &count)
          == UMI_STATUS_INVALID_STATE);
    CHECK(UmiAiTrainingAdamWCheckpointSize(model, adam, &count)
          == UMI_STATUS_OK);
    bytes = malloc(count);
    CHECK(bytes != NULL);
    memset(bytes, 0x5a, count);
    CHECK(UmiAiTrainingAdamWCheckpointEncode(model, adam,
          bytes, count - 1U, &used) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(used == 99U && bytes[0] == 0x5a);
    CHECK(UmiAiTrainingAdamWCheckpointEncode(model, adam,
          bytes, count, &used) == UMI_STATUS_OK);
    CHECK(used == count);
    UmiAiTrainingDecoder *const oldModel = other;
    UmiAiTrainingAdamW *const oldAdam = adam;
    untrusted = other;
    untrustedAdam = adam;
    CHECK(UmiAiTrainingAdamWCheckpointDecode(bytes, count - 1U,
          &untrusted, &untrustedAdam) != UMI_STATUS_OK);
    CHECK(untrusted == oldModel && untrustedAdam == oldAdam);
    bytes[33U] ^= 0x40U;
    CHECK(UmiAiTrainingAdamWCheckpointDecode(bytes, count,
          &untrusted, &untrustedAdam) != UMI_STATUS_OK);
    CHECK(untrusted == oldModel && untrustedAdam == oldAdam);
    bytes[33U] ^= 0x40U;
    bytes[count - 1U] ^= 0x01U;
    CHECK(UmiAiTrainingAdamWCheckpointDecode(bytes, count,
          &untrusted, &untrustedAdam) != UMI_STATUS_OK);
    CHECK(untrusted == oldModel && untrustedAdam == oldAdam);
    bytes[count - 1U] ^= 0x01U;
    size_t tokens[] = {1U,2U,3U}, targets[] = {2U,3U,4U};
    float loss = 0.0F;
    CHECK(UmiAiTrainingDecoderTrainStep(model, tokens, targets, 3U,
          0.01F, &loss) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingAdamWCheckpointSize(model, adam, &count)
          == UMI_STATUS_INVALID_STATE);
    free(bytes);
    UmiAiTrainingAdamWDestroy(adam);
    UmiAiTrainingDecoderDestroy(other);
    UmiAiTrainingDecoderDestroy(model);
    puts("AdamW checkpoint validation, output atomicity and stale bindings: PASS");
    return 0;
}
