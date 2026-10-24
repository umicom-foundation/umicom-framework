/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_rotary_real_tape_rewind.c
 * PURPOSE:
 *   Reuse the same canonical tape through 48 complete forward/backward
 *   iterations, retaining leaf tensors and removing every intermediate node.
 *   Validate tracked allocation stability and gradient ownership, while
 *   explicitly not claiming to measure process RSS or every allocator leak.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/rotary_tape.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { \
    fprintf(stderr,"real-tape-rewind line %d: %s\n",__LINE__,#expr); \
    return 1; \
} } while (0)

int main(void)
{
    static const float QUERY[12] = {
        .4F,.1F,.3F,.5F, .2F,.8F,.1F,.6F, .6F,.3F,.7F,.2F
    };
    static const float KEY[12] = {
        .2F,.4F,.8F,.1F, .3F,.7F,.2F,.5F, .4F,.1F,.6F,.8F
    };
    static const float VALUE[12] = {
        .1F,.6F,.3F,.2F, .2F,.7F,.1F,.4F, .8F,.2F,.5F,.3F
    };
    static const float TARGET[12] = {
        .12F,.58F,.29F,.25F, .35F,.55F,.23F,.30F, .60F,.36F,.41F,.33F
    };
    const UmiAiTrainingRotaryConfig rotary = {2U, 2U, 10000.0};
    UmiAiTrainingTape *tape = NULL;
    UmiAiTrainingTensor *query = NULL,*key = NULL,*value = NULL,*target = NULL;
    UmiAiTrainingTapeMemory leaves = {0},during = {0},after = {0};
    size_t checkpoint = 0U, count = 0U, repeatedPeak = 0U;
    float firstLoss = 0.0F;
    CHECK(UmiAiTrainingTapeCreate(&tape) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape,3U,4U,QUERY,12U,1,&query) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape,3U,4U,KEY,12U,1,&key) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape,3U,4U,VALUE,12U,0,&value) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape,3U,4U,TARGET,12U,0,&target) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTapeCheckpoint(tape,&checkpoint) == UMI_STATUS_OK);
    CHECK(checkpoint == 4U);
    CHECK(UmiAiTrainingTapeMemoryInspect(tape,&leaves) == UMI_STATUS_OK);
    for (size_t iteration = 0U; iteration < 48U; ++iteration) {
        UmiAiTrainingTensor *attended = NULL,*loss = NULL;
        const uint64_t position = 11U + (uint64_t)(iteration % 6U) * 23U;
        float lossValue=0.0F, qGradient[12],kGradient[12];
        CHECK(UmiAiTrainingRotaryTapeAttention(tape,query,key,value,
              &rotary,3U,2U,&position,1U,&attended) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingMeanSquaredError(tape,attended,target,&loss) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingTensorValues(loss,&lossValue,1U) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingBackward(tape,loss) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingTensorGradients(query,qGradient,12U) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingTensorGradients(key,kGradient,12U) == UMI_STATUS_OK);
        CHECK(isfinite(lossValue));
        for (size_t j=0U;j<12U;++j) {
            CHECK(isfinite(qGradient[j]));
            CHECK(isfinite(kGradient[j]));
        }
        CHECK(UmiAiTrainingTapeMemoryInspect(tape,&during) == UMI_STATUS_OK);
        CHECK(during.currentBytes > leaves.currentBytes);
        CHECK(during.tensorCount > leaves.tensorCount);
        if (iteration == 0U) {
            firstLoss = lossValue;
            repeatedPeak = during.peakBytes;
        } else {
            CHECK(fabsf(lossValue-firstLoss) < 0.00004F);
            CHECK(during.peakBytes == repeatedPeak);
        }
        CHECK(UmiAiTrainingTapeRewind(tape,checkpoint) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingTapeMemoryInspect(tape,&after) == UMI_STATUS_OK);
        CHECK(after.tensorCount == leaves.tensorCount);
        CHECK(after.currentBytes == leaves.currentBytes);
        CHECK(after.peakBytes == repeatedPeak);
        CHECK(UmiAiTrainingTapeCheckpoint(tape,&count) == UMI_STATUS_OK);
        CHECK(count == checkpoint);
    }
    UmiAiTrainingTapeDestroy(tape);
    printf("real Framework tape: 48 forwards, 48 backwards, 48 rewinds; peak tracked %zu bytes PASS\n",
           repeatedPeak);
    return 0;
}
