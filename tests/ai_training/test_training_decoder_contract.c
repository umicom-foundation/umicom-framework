/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_decoder_contract.c
 * Purpose: Reject bad decoder shapes, verify positional/causal isolation.
 * Author: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/decoder.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); return 1; } } while (0)
int main(void)
{
    UmiAiTrainingDecoderConfig cfg = {5U, 3U, 4U, 8U, 2U, UINT64_C(42)};
    UmiAiTrainingDecoder *a = NULL, *b = NULL;
    size_t seqA[3] = {0,1,2}, seqB[3] = {0,1,4};
    float logitsA[15], logitsB[15], output[15];
    float loss = -99.0F;
    uint64_t steps = 999U;
    UmiAiTrainingDecoderConfig readCfg = {0};
    cfg.attentionHeads = 3U;
    CHECK(UmiAiTrainingDecoderCreate(&cfg, &a) == UMI_STATUS_INVALID_ARGUMENT && a == NULL);
    cfg.attentionHeads = 2U;
    CHECK(UmiAiTrainingDecoderCreate(&cfg, &a) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCreate(&cfg, &b) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderLogits(a, seqA, 3, logitsA, 15) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderLogits(b, seqA, 3, output, 15) == UMI_STATUS_OK);
    for (size_t i=0; i<15; ++i) CHECK(logitsA[i] == output[i] && isfinite(logitsA[i]));
    CHECK(UmiAiTrainingDecoderLogits(a, seqB, 3, logitsB, 15) == UMI_STATUS_OK);
    for (size_t i=0; i<10; ++i) CHECK(logitsA[i] == logitsB[i]);
    /* Changing an unrelated batched sequence cannot affect the first one. */
    size_t batchA[6] = {0U, 1U, 2U, 3U, 1U, 0U};
    size_t batchB[6] = {0U, 1U, 2U, 0U, 0U, 0U};
    float batchLogitsA[30], batchLogitsB[30];
    CHECK(UmiAiTrainingDecoderLogits(a,batchA,6,batchLogitsA,30) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderLogits(a,batchB,6,batchLogitsB,30) == UMI_STATUS_OK);
    for (size_t i=0; i<15; ++i) CHECK(batchLogitsA[i] == batchLogitsB[i]);
    for (size_t i=0; i<15; ++i) output[i] = -173.0F;
    CHECK(UmiAiTrainingDecoderLogits(a, seqA, 3, output, 14) == UMI_STATUS_CAPACITY_EXCEEDED);
    for (size_t i=0; i<15; ++i) CHECK(output[i] == -173.0F);
    CHECK(UmiAiTrainingDecoderEvaluate(a, seqA, NULL, 3, &loss) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(loss == -99.0F);
    CHECK(UmiAiTrainingDecoderInspect(a, &readCfg, &steps) == UMI_STATUS_OK);
    CHECK(steps == 0U && readCfg.contextLength == cfg.contextLength);
    UmiAiTrainingDecoderDestroy(a);
    UmiAiTrainingDecoderDestroy(b);
    puts("Decoder contract and causal isolation: passed");
    return 0;
}
