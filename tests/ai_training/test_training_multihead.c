/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_multihead.c
 * PURPOSE: Test multi-head causal masking, per-head channel isolation and
 *          batch independence against manually calculated probabilities.
 * AUTHOR: Sammy Hegab / Umicom Foundation | LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/tape.h"
#include <math.h>
#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { \
    fprintf(stderr,"multihead verification line %d: %s\n",__LINE__,#expr);return 1; \
} } while (0)

int main(void)
{
    UmiAiTrainingTape *tape = NULL;
    UmiAiTrainingTensor *q = NULL, *k = NULL, *v = NULL, *out = NULL;
    /* Two sequences, each of length two and width four. Q and K are zero,
     * so each head's softmax is exactly uniform over allowed history. */
    const float zero[16] = {0.0F};
    const float values[16] = {
        2.0F, 4.0F, 10.0F, 20.0F,
        6.0F, 8.0F, 30.0F, 40.0F,
        100.0F, 200.0F, 500.0F, 700.0F,
        300.0F, 400.0F, 900.0F, 1100.0F
    };
    const float expected[16] = {
        2.0F, 4.0F, 10.0F, 20.0F,
        4.0F, 6.0F, 20.0F, 30.0F,
        100.0F, 200.0F, 500.0F, 700.0F,
        200.0F, 300.0F, 700.0F, 900.0F
    };
    float observed[16] = {0.0F};
    UmiAiTrainingTensor *sentinel = (UmiAiTrainingTensor *)&expected;
    CHECK(UmiAiTrainingTapeCreate(&tape) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape, 4U, 4U, zero, 16U, 0, &q) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape, 4U, 4U, zero, 16U, 0, &k) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape, 4U, 4U, values, 16U, 0, &v) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingMultiHeadCausalAttention(tape, q,k,v,2U,2U,&out) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorValues(out,observed,16U) == UMI_STATUS_OK);
    for (size_t i=0U;i<16U;++i) CHECK(fabsf(observed[i]-expected[i])<0.00015F);
    CHECK(UmiAiTrainingMultiHeadCausalAttention(tape,q,k,v,2U,3U,&sentinel)
          == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(sentinel == (UmiAiTrainingTensor *)&expected);
    CHECK(UmiAiTrainingMultiHeadCausalAttention(tape,q,k,v,0U,2U,&sentinel)
          == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingMultiHeadCausalAttention(tape,q,k,v,3U,2U,&sentinel)
          == UMI_STATUS_INVALID_ARGUMENT);
    UmiAiTrainingTapeDestroy(tape);
    puts("multi-head forward masking and batch separation passed");
    return 0;
}
