/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_transformer_low_rank.c
 * PURPOSE:
 *   Qualify optional attention and feed-forward low-rank projections against
 *   the original Transformer block, with frozen bases and tape rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "transformer_fixture.h"
#include "umicom/ai/training/transformer_low_rank.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { \
    if (!(expr)) { \
        fprintf(stderr, "Transformer adaptation line %d: %s\n", __LINE__, #expr); \
        return 1; \
    } \
} while (0)

int main(void)
{
    UmiTransformerTestData data;
    UmiTransformerTestFixture fixture;
    UmiAiTrainingLowRankProjection query = {0}, feedForwardUp = {0};
    UmiAiTrainingTransformerLowRankBindings bindings = {0};
    UmiAiTrainingTensor *canonical = NULL, *adapted = NULL, *goal = NULL, *loss = NULL;
    UmiAiTrainingTensor *trainables[UMI_AI_TRANSFORMER_LOW_RANK_MAX_TRAINABLES] = {0};
    size_t trainableCount = 0U, retainedNodes = 0U, observedNodes = 0U;
    const float desired[16] = {
        0.6F, -0.5F, 0.2F, 0.9F,
       -0.1F, 0.1F, 0.3F, 0.4F,
        0.1F, -0.2F, 0.7F, 0.2F,
        0.3F, 0.5F, 0.1F, -0.4F
    };
    float before[16] = {0}, initial[16] = {0}, trained[16] = {0};
    float frozenQuery[16] = {0}, afterQuery[16] = {0};
    float firstLoss = 0.0F, lastLoss = 0.0F;
    UmiStatus status;

    InitialiseTestData(&data);
    CHECK(CreateTestFixture(&fixture, &data) == UMI_STATUS_OK);

    /* Copy the exact canonical weights into each frozen adapter base. */
    CHECK(UmiAiTrainingLowRankCreate(fixture.tape, data.values[T_QUERY],
        T_CHANNELS, T_CHANNELS, 2U, UINT64_C(314), &query) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingLowRankCreate(fixture.tape, data.values[T_FF_UP],
        T_CHANNELS, T_HIDDEN, 2U, UINT64_C(159), &feedForwardUp) == UMI_STATUS_OK);
    bindings.query = &query;
    bindings.feedForwardUp = &feedForwardUp;

    CHECK(UmiAiTrainingTransformerLowRankTrainables(
        &bindings, trainables, 4U, &trainableCount) == UMI_STATUS_OK);
    CHECK(trainableCount == 4U);
    CHECK(UmiAiTrainingTensorValues(fixture.parameters[T_QUERY],
        frozenQuery, 16U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTapeCheckpoint(fixture.tape, &retainedNodes) == UMI_STATUS_OK);

    CHECK(UmiAiTrainingTransformerBlockForward(fixture.tape,
        fixture.input, &fixture.block, &fixture.config, &canonical) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorValues(canonical, before, 16U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTapeRollback(fixture.tape, retainedNodes) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTransformerBlockForwardLowRank(fixture.tape,
        fixture.input, &fixture.block, &fixture.config,
        &bindings, &adapted) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorValues(adapted, initial, 16U) == UMI_STATUS_OK);
    for (size_t i = 0U; i < 16U; ++i)
        CHECK(fabsf(before[i] - initial[i]) < 0.00002F);
    CHECK(UmiAiTrainingTapeRollback(fixture.tape, retainedNodes) == UMI_STATUS_OK);

    /* Never accept a shape-identical adapter copied from other base weights. */
    UmiAiTrainingLowRankProjection foreign = {0};
    float wrong[16];
    memcpy(wrong, data.values[T_KEY], sizeof(wrong));
    wrong[0] += 0.5F;
    CHECK(UmiAiTrainingLowRankCreate(fixture.tape, wrong,
        T_CHANNELS, T_CHANNELS, 2U, UINT64_C(3), &foreign) == UMI_STATUS_OK);
    /* The additional leaves remain in the tape and must be retained until
     * the final destruction, so update the rollback point. */
    CHECK(UmiAiTrainingTapeCheckpoint(fixture.tape, &retainedNodes) == UMI_STATUS_OK);
    bindings.query = &foreign;
    adapted = fixture.input;
    CHECK(UmiAiTrainingTransformerBlockForwardLowRank(fixture.tape,
        fixture.input, &fixture.block, &fixture.config,
        &bindings, &adapted) == UMI_STATUS_INVALID_STATE);
    CHECK(adapted == fixture.input);
    CHECK(UmiAiTrainingTapeCheckpoint(fixture.tape, &observedNodes) == UMI_STATUS_OK);
    CHECK(observedNodes == retainedNodes);
    bindings.query = &query;

    /* A training target and all adapted leaves are retained; only transient
     * forward, loss and derivative nodes are discarded after each update. */
    CHECK(UmiAiTrainingTensorCreate(fixture.tape, 4U, 4U, desired, 16U,
        0, &goal) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTapeCheckpoint(fixture.tape, &retainedNodes) == UMI_STATUS_OK);
    for (size_t step = 0U; step < 160U; ++step) {
        adapted = NULL;
        CHECK(UmiAiTrainingTransformerBlockForwardLowRank(fixture.tape,
            fixture.input, &fixture.block, &fixture.config,
            &bindings, &adapted) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingMeanSquaredError(fixture.tape,
            adapted, goal, &loss) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingTensorValues(loss, &lastLoss, 1U) == UMI_STATUS_OK);
        CHECK(isfinite(lastLoss));
        if (step == 0U) firstLoss = lastLoss;
        CHECK(UmiAiTrainingBackward(fixture.tape, loss) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingSgdStepMany(fixture.tape,
            trainables, trainableCount, 0.12F) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingTapeRollback(fixture.tape, retainedNodes) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingTapeCheckpoint(fixture.tape, &observedNodes) == UMI_STATUS_OK);
        CHECK(observedNodes == retainedNodes);
    }

    /* Verify actual learned output, not just that a gradient existed. */
    CHECK(UmiAiTrainingTransformerBlockForwardLowRank(fixture.tape,
        fixture.input, &fixture.block, &fixture.config,
        &bindings, &adapted) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorValues(adapted, trained, 16U) == UMI_STATUS_OK);
    status = UmiAiTrainingMeanSquaredError(fixture.tape,
        adapted, goal, &loss);
    CHECK(status == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorValues(loss, &lastLoss, 1U) == UMI_STATUS_OK);
    CHECK(lastLoss < firstLoss);
    CHECK(UmiAiTrainingTensorValues(fixture.parameters[T_QUERY],
        afterQuery, 16U) == UMI_STATUS_OK);
    CHECK(memcmp(frozenQuery, afterQuery, sizeof(frozenQuery)) == 0);
    CHECK(UmiAiTrainingTapeRollback(fixture.tape, retainedNodes) == UMI_STATUS_OK);

    FreeTestFixture(&fixture);
    printf("Transformer projection adaptation MSE %.7f -> %.7f; "
           "frozen canonical weights preserved\n",
           (double)firstLoss, (double)lastLoss);
    return 0;
}
