/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_single_query_tape_parity.c
 *
 * PURPOSE:
 *   Single-query versus real canonical tape forward.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/single_query_attention.h"
#include "umicom/ai/training/tape.h"
#include <math.h>
#include <stdio.h>

#define CHECK(value) do {if(!(value)){fprintf(stderr,"%s:%d: %s\n", \
    __FILE__,__LINE__,#value);return 1;}}while(0)

int main(void)
{
    const size_t rows = 3U, channels = 4U, heads = 2U;
    const float q[] = {0.1F, 0.2F, 0.3F, 0.4F,
                       -0.4F, 0.5F, -0.3F, 0.9F,
                        0.5F, -0.25F, 0.8F, 0.6F};
    const float k[] = {0.2F, 0.5F, -0.1F, 0.7F,
                       0.3F, 0.8F, 0.4F, 0.1F,
                      -0.4F, 0.3F, 0.6F, -0.2F};
    const float v[] = {1.0F, 2.0F, 3.0F, 4.0F,
                      -1.0F, 4.0F, 0.5F, 1.2F,
                       0.0F, -1.0F, 2.0F, -2.0F};
    float last[4] = {0};
    float canonical[12] = {0};
    UmiAiTrainingTape *tape = NULL;
    UmiAiTrainingTensor *qt = NULL, *kt = NULL, *vt = NULL, *attn = NULL;
    CHECK(UmiAiTrainingTapeCreate(&tape) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape, rows, channels, q, 12U, 0, &qt) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape, rows, channels, k, 12U, 0, &kt) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape, rows, channels, v, 12U, 0, &vt) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingMultiHeadCausalAttention(tape, qt, kt, vt,
        rows, heads, &attn) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorValues(attn, canonical, 12U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingSingleQueryAttentionForward(q + 8U, 4U, k, 12U, v, 12U,
        rows, channels, heads, last, 4U, NULL) == UMI_STATUS_OK);
    for (size_t i = 0U; i < channels; ++i)
        CHECK(fabs((double)last[i] - (double)canonical[8U + i]) < 1e-6);
    UmiAiTrainingTapeDestroy(tape);
    puts("candidate and canonical final attention row agree");
    return 0;
}
