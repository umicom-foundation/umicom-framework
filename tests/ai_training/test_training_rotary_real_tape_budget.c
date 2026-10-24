/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_rotary_real_tape_budget.c
 * PURPOSE:
 *   Verify bounded failure and rollback against the production tape memory
 *   budget and graph. A rejected rotary composition must not publish a node,
 *   change retained leaf values, or leave tape memory allocated to lost nodes.
 *   This test deliberately does not replace the Framework allocator.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/rotary_tape.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { \
    fprintf(stderr, "real-tape-budget line %d: %s\n", __LINE__, #expr); \
    return 1; \
} } while (0)

int main(void)
{
    static const float QUERY[12] = {
        .3F,.2F,.5F,.1F, .4F,.8F,.2F,.1F, .5F,.4F,.2F,.7F
    };
    static const float KEY[12] = {
        .2F,.6F,.3F,.5F, .7F,.1F,.3F,.4F, .5F,.2F,.8F,.1F
    };
    static const float VALUE[12] = {
        .1F,.2F,.7F,.3F, .4F,.1F,.2F,.5F, .8F,.3F,.1F,.6F
    };
    const UmiAiTrainingRotaryConfig rotary = {2U, 2U, 10000.0};
    const uint64_t position = 97U;
    UmiAiTrainingTape *tape = NULL;
    UmiAiTrainingTensor *query = NULL, *key = NULL, *value = NULL;
    UmiAiTrainingTensor *result = NULL;
    UmiAiTrainingTapeMemory baseline = {0}, after = {0};
    float valuesAfter[12] = {0};
    size_t checkpoint = 0U, countAfter = 0U;
    CHECK(UmiAiTrainingTapeCreate(&tape) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape,3U,4U,QUERY,12U,1,&query) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape,3U,4U,KEY,12U,1,&key) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape,3U,4U,VALUE,12U,0,&value) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTapeCheckpoint(tape,&checkpoint) == UMI_STATUS_OK);
    CHECK(checkpoint == 3U);
    CHECK(UmiAiTrainingTapeMemoryInspect(tape,&baseline) == UMI_STATUS_OK);
    CHECK(baseline.tensorCount == 3U);
    CHECK(baseline.currentBytes < SIZE_MAX - 512U);

    /* Deliberately exercise failures after partial composed-node creation.
     * Only the Framework's public tape budget is used, not injected mocks. */
    const size_t allowances[] = {1U, 128U, 512U};
    for (size_t i=0U;i<sizeof allowances/sizeof allowances[0];++i) {
        result = query; /* known valid sentinel handle, never dereferenced as result */
        CHECK(UmiAiTrainingTapeSetMemoryBudget(tape,
              baseline.currentBytes + allowances[i]) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingRotaryTapeAttention(tape, query, key, value,
              &rotary,3U,2U,&position,1U,&result) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(result == query);
        CHECK(UmiAiTrainingTapeCheckpoint(tape,&countAfter) == UMI_STATUS_OK);
        CHECK(countAfter == checkpoint);
        CHECK(UmiAiTrainingTapeMemoryInspect(tape,&after) == UMI_STATUS_OK);
        CHECK(after.currentBytes == baseline.currentBytes);
        CHECK(after.tensorCount == baseline.tensorCount);
        CHECK(after.budgetBytes == baseline.currentBytes + allowances[i]);
        CHECK(UmiAiTrainingTensorValues(query,valuesAfter,12U) == UMI_STATUS_OK);
        CHECK(memcmp(valuesAfter,QUERY,sizeof QUERY) == 0);
    }
    CHECK(UmiAiTrainingTapeSetMemoryBudget(tape,0U) == UMI_STATUS_OK);
    result = NULL;
    CHECK(UmiAiTrainingRotaryTapeAttention(tape,query,key,value,
          &rotary,3U,2U,&position,1U,&result) == UMI_STATUS_OK);
    CHECK(result != NULL);
    CHECK(UmiAiTrainingTapeMemoryInspect(tape,&after) == UMI_STATUS_OK);
    CHECK(after.currentBytes > baseline.currentBytes);
    CHECK(after.tensorCount > baseline.tensorCount);
    CHECK(UmiAiTrainingTapeRollback(tape,checkpoint) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTapeMemoryInspect(tape,&after) == UMI_STATUS_OK);
    CHECK(after.currentBytes == baseline.currentBytes);
    CHECK(after.tensorCount == baseline.tensorCount);

    /* Invalid absolute offset is detected without graph mutation. */
    const uint64_t invalidPosition = UMI_AI_ROTARY_MAX_POSITION;
    result = query;
    CHECK(UmiAiTrainingRotaryTapeAttention(tape,query,key,value,
          &rotary,3U,2U,&invalidPosition,1U,&result) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(result == query);
    CHECK(UmiAiTrainingTapeCheckpoint(tape,&countAfter) == UMI_STATUS_OK);
    CHECK(countAfter == checkpoint);
    UmiAiTrainingTapeDestroy(tape);
    puts("real Framework tape: bounded memory rejection, exact rollback and recovery PASS");
    return 0;
}
