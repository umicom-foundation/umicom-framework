/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_decoder_train.c
 * Purpose: Repeatably train the complete decoder and evaluate held-out groups.
 * Author: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/decoder.h"
#include "umicom/ai/training/dataset_split.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); return 1; } } while (0)
int main(void)
{
    UmiAiTrainingDecoderConfig cfg = {4U, 2U, 4U, 8U, 2U, UINT64_C(1001)};
    UmiAiTrainingDecoder *model = NULL;
    size_t ids[12], inputs[18], targets[18], valIn[6], valOut[6];
    float before = 0.0F, after = 0.0F, heldout = 0.0F;
    uint64_t steps = 0;
    UmiAiTrainingDecoderConfig info;
    CHECK(UmiAiTrainingDecoderCreate(&cfg,&model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingSplitSequences(12, 3, UINT64_C(69), ids, 12) == UMI_STATUS_OK);
    /* Twelve distinct ordered token pairs, excluding the four (x,x) pairs.
     * The withheld examples are unseen combinations; the small corpus is
     * NOT evidence of general language understanding. */
    for(size_t i=0;i<12;++i){
        size_t id = ids[i];
        size_t a = id / 3U;
        size_t b = id % 3U;
        if (b >= a) b++;
        size_t *dstIn = i < 9U ? inputs + 2U*i : valIn + 2U*(i-9U);
        size_t *dstOut = i < 9U ? targets + 2U*i : valOut + 2U*(i-9U);
        dstIn[0] = a; dstIn[1] = b;
        dstOut[0] = b; dstOut[1] = a;
    }
    CHECK(UmiAiTrainingDecoderEvaluate(model, inputs, targets, 18, &before) == UMI_STATUS_OK);
    for (size_t s=0;s<260;++s)
        CHECK(UmiAiTrainingDecoderTrainStep(model, inputs, targets, 18, 0.08F, &after) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderEvaluate(model, inputs, targets, 18, &after) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderEvaluate(model, valIn, valOut, 6, &heldout) == UMI_STATUS_OK);
    CHECK(isfinite(before) && isfinite(after) && isfinite(heldout));
    CHECK(after < before);
    CHECK(UmiAiTrainingDecoderInspect(model, &info, &steps) == UMI_STATUS_OK && steps == 260U);
    float unchanged = 99.0F;
    size_t badTarget[2] = {4U, 1U};
    CHECK(UmiAiTrainingDecoderTrainStep(model, inputs, badTarget, 2, 0.1F,
                                      &unchanged) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(unchanged == 99.0F);
    CHECK(UmiAiTrainingDecoderInspect(model, &info, &steps) == UMI_STATUS_OK && steps == 260U);
    UmiAiTrainingDecoderDestroy(model);
    puts("Decoder training and held-out evaluation: passed");
    return 0;
}
