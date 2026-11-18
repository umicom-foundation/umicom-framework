/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/training_loss/test_masked_squared_error.c
 *
 * PURPOSE:
 *   Verify element selection, independent gradients, finite arithmetic and owned tape lifetime for squared error.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/tape.h"
#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)
static int Near(float actual, double expected, double tolerance)
{ return fabs((double)actual - expected) <= tolerance; }
static int Evaluate(const float *p, const float *t, const unsigned char *mask,
    float *value, float *pg, float *tg)
{
    UmiAiTrainingTape *tape = NULL;
    UmiAiTrainingTensor *prediction = NULL, *target = NULL, *loss = NULL;
    CHECK(UmiAiTrainingTapeCreate(&tape) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape, 2U, 3U, p, 6U, 1, &prediction) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape, 2U, 3U, t, 6U, 1, &target) == UMI_STATUS_OK);
    CHECK((mask ? UmiAiTrainingMaskedMeanSquaredError(tape, prediction, target, mask, 6U, &loss) :
        UmiAiTrainingMeanSquaredError(tape, prediction, target, &loss)) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorValues(loss, value, 1U) == UMI_STATUS_OK);
    if (pg != NULL || tg != NULL) {
        CHECK(UmiAiTrainingBackward(tape, loss) == UMI_STATUS_OK);
        if (pg != NULL) CHECK(UmiAiTrainingTensorGradients(prediction, pg, 6U) == UMI_STATUS_OK);
        if (tg != NULL) CHECK(UmiAiTrainingTensorGradients(target, tg, 6U) == UMI_STATUS_OK);
    }
    UmiAiTrainingTapeDestroy(tape);
    return 0;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    float p[6] = {1, 2, 9, -1, 4, 12}, t[6] = {0, 0, 0, 1, 1, 0};
    unsigned char mask[6] = {1, 1, 0, 1, 1, 0};
    float value, pg[6], tg[6];
    if (!strcmp(mode, "reference")) {
        CHECK(!Evaluate(p, t, mask, &value, pg, tg));
        CHECK(value == 4.5F);
        const float expected[6] = {0.5F, 1.0F, 0.0F, -1.0F, 1.5F, 0.0F};
        for (size_t i = 0U; i < 6U; ++i) CHECK(pg[i] == expected[i] && tg[i] == -expected[i]);
    } else if (!strcmp(mode, "finite-difference")) {
        CHECK(!Evaluate(p, t, mask, &value, pg, tg));
        for (size_t operand = 0U; operand < 2U; ++operand) {
            float *values = operand == 0U ? p : t;
            const float *gradients = operand == 0U ? pg : tg;
            for (size_t i = 0U; i < 6U; ++i) {
                const float original = values[i]; float plus, minus;
                values[i] = original + 0.001F; CHECK(!Evaluate(p, t, mask, &plus, NULL, NULL));
                values[i] = original - 0.001F; CHECK(!Evaluate(p, t, mask, &minus, NULL, NULL));
                values[i] = original;
                CHECK(Near(gradients[i], ((double)plus - minus) / 0.002, 0.0005));
            }
        }
    } else if (!strcmp(mode, "all-selected")) {
        float other, otherPg[6], otherTg[6]; memset(mask, 1, sizeof mask);
        for (int k = -20; k <= 20; ++k) {
            p[0] = (float)k / 4.0F; t[4] = (float)(k + 3) / 8.0F;
            CHECK(!Evaluate(p, t, NULL, &value, pg, tg));
            CHECK(!Evaluate(p, t, mask, &other, otherPg, otherTg));
            CHECK(value == other && !memcmp(pg, otherPg, sizeof pg) && !memcmp(tg, otherTg, sizeof tg));
        }
    } else if (!strcmp(mode, "excluded-extremes")) {
        p[2] = FLT_MAX; t[2] = -FLT_MAX; p[5] = -FLT_MAX; t[5] = FLT_MAX;
        CHECK(!Evaluate(p, t, mask, &value, pg, tg));
        CHECK(value == 4.5F && pg[2] == 0 && pg[5] == 0 && tg[2] == 0 && tg[5] == 0);
    } else if (!strcmp(mode, "full-capacity")) {
        const size_t count = UMI_AI_TRAINING_MAX_ELEMENTS;
        float *values = calloc(count, sizeof *values), *gradients = calloc(count, sizeof *gradients);
        unsigned char *selected = calloc(count, 1U);
        CHECK(values != NULL && gradients != NULL && selected != NULL);
        UmiAiTrainingTape *tape = NULL; UmiAiTrainingTensor *prediction = NULL, *target = NULL, *loss = NULL;
        CHECK(UmiAiTrainingTapeCreate(&tape) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingTensorCreate(tape, 1U, count, values, count, 0, &target) == UMI_STATUS_OK);
        values[count - 1U] = 3; selected[count - 1U] = 1U;
        CHECK(UmiAiTrainingTensorCreate(tape, 1U, count, values, count, 1, &prediction) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingMaskedMeanSquaredError(tape, prediction, target, selected, count, &loss) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingTensorValues(loss, &value, 1U) == UMI_STATUS_OK && value == 9);
        CHECK(UmiAiTrainingBackward(tape, loss) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingTensorGradients(prediction, gradients, count) == UMI_STATUS_OK);
        for (size_t i = 0U; i + 1U < count; ++i) CHECK(gradients[i] == 0);
        CHECK(gradients[count - 1U] == 6);
        free(selected); free(gradients); free(values); UmiAiTrainingTapeDestroy(tape);
    } else {
        UmiAiTrainingTape *tape = NULL, *other = NULL;
        UmiAiTrainingTensor *prediction = NULL, *target = NULL, *loss = NULL, *foreign = NULL;
        UmiAiTrainingTapeMemory before, after;
        CHECK(UmiAiTrainingTapeCreate(&tape) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingTensorCreate(tape, 2U, 3U, p, 6U, 1, &prediction) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingTensorCreate(tape, 2U, 3U, t, 6U, 1, &target) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingTapeMemoryInspect(tape, &before) == UMI_STATUS_OK); loss = prediction;
        if (!strcmp(mode, "owned-selection")) {
            unsigned char *owned = malloc(sizeof mask); CHECK(owned != NULL); memcpy(owned, mask, sizeof mask);
            CHECK(UmiAiTrainingMaskedMeanSquaredError(tape, prediction, target, owned, 6U, &loss) == UMI_STATUS_OK);
            memset(owned, 0, sizeof mask); free(owned);
            CHECK(UmiAiTrainingBackward(tape, loss) == UMI_STATUS_OK);
            CHECK(UmiAiTrainingTensorGradients(prediction, pg, 6U) == UMI_STATUS_OK);
            CHECK(pg[0] == 0.5F && pg[2] == 0 && pg[4] == 1.5F);
            CHECK(UmiAiTrainingTapeMemoryInspect(tape, &after) == UMI_STATUS_OK);
            CHECK(after.auxiliaryBytes - before.auxiliaryBytes == 6U);
            CHECK(UmiAiTrainingTapeRewind(tape, 2U) == UMI_STATUS_OK);
            CHECK(UmiAiTrainingTapeMemoryInspect(tape, &after) == UMI_STATUS_OK);
            CHECK(after.currentBytes == before.currentBytes && after.auxiliaryBytes == before.auxiliaryBytes);
        } else if (!strcmp(mode, "budget")) {
            CHECK(UmiAiTrainingMaskedMeanSquaredError(tape, prediction, target, mask, 6U, &loss) == UMI_STATUS_OK);
            CHECK(UmiAiTrainingTapeMemoryInspect(tape, &after) == UMI_STATUS_OK);
            const size_t exactBudget = after.currentBytes;
            CHECK(UmiAiTrainingTapeRewind(tape, 2U) == UMI_STATUS_OK);
            CHECK(UmiAiTrainingTapeSetMemoryBudget(tape, exactBudget - 1U) == UMI_STATUS_OK);
            CHECK(UmiAiTrainingTapeMemoryInspect(tape, &before) == UMI_STATUS_OK); loss = prediction;
            CHECK(UmiAiTrainingMaskedMeanSquaredError(tape, prediction, target, mask, 6U, &loss) == UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(loss == prediction && UmiAiTrainingTapeMemoryInspect(tape, &after) == UMI_STATUS_OK);
            CHECK(!memcmp(&before, &after, sizeof before));
            CHECK(UmiAiTrainingTapeSetMemoryBudget(tape, exactBudget) == UMI_STATUS_OK);
            CHECK(UmiAiTrainingMaskedMeanSquaredError(tape, prediction, target, mask, 6U, &loss) == UMI_STATUS_OK);
        } else if (!strcmp(mode, "rejections")) {
            unsigned char none[6] = {0}, bad[6] = {1, 0, 0, 0, 0, 2};
            CHECK(UmiAiTrainingMaskedMeanSquaredError(tape, prediction, target, none, 6U, &loss) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiAiTrainingMaskedMeanSquaredError(tape, prediction, target, bad, 6U, &loss) == UMI_STATUS_INVALID_ARGUMENT);
            const size_t counts[] = {0U, 5U, 7U, SIZE_MAX};
            for (size_t i = 0U; i < sizeof counts / sizeof counts[0]; ++i)
                CHECK(UmiAiTrainingMaskedMeanSquaredError(tape, prediction, target, mask, counts[i], &loss) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(loss == prediction && UmiAiTrainingTapeMemoryInspect(tape, &after) == UMI_STATUS_OK);
            CHECK(!memcmp(&before, &after, sizeof before));
        } else if (!strcmp(mode, "arguments")) {
            CHECK(UmiAiTrainingMaskedMeanSquaredError(NULL, prediction, target, mask, 6U, &loss) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiAiTrainingMaskedMeanSquaredError(tape, NULL, target, mask, 6U, &loss) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiAiTrainingMaskedMeanSquaredError(tape, prediction, NULL, mask, 6U, &loss) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiAiTrainingMaskedMeanSquaredError(tape, prediction, target, NULL, 6U, &loss) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiAiTrainingMaskedMeanSquaredError(tape, prediction, target, mask, 6U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiAiTrainingTapeCreate(&other) == UMI_STATUS_OK);
            CHECK(UmiAiTrainingTensorCreate(other, 2U, 3U, t, 6U, 0, &foreign) == UMI_STATUS_OK);
            CHECK(UmiAiTrainingMaskedMeanSquaredError(tape, foreign, target, mask, 6U, &loss) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiAiTrainingMaskedMeanSquaredError(tape, prediction, foreign, mask, 6U, &loss) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(loss == prediction && UmiAiTrainingTapeMemoryInspect(tape, &after) == UMI_STATUS_OK);
            CHECK(!memcmp(&before, &after, sizeof before));
        } else if (!strcmp(mode, "shape")) {
            CHECK(UmiAiTrainingTensorCreate(tape, 1U, 6U, t, 6U, 0, &foreign) == UMI_STATUS_OK);
            CHECK(UmiAiTrainingTapeMemoryInspect(tape, &before) == UMI_STATUS_OK);
            CHECK(UmiAiTrainingMaskedMeanSquaredError(tape, prediction, foreign, mask, 6U, &loss) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(loss == prediction && UmiAiTrainingTapeMemoryInspect(tape, &after) == UMI_STATUS_OK);
            CHECK(!memcmp(&before, &after, sizeof before));
        } else if (!strcmp(mode, "numeric-refusal")) {
            p[0] = FLT_MAX;
            CHECK(UmiAiTrainingTensorCreate(tape, 2U, 3U, p, 6U, 1, &foreign) == UMI_STATUS_OK);
            CHECK(UmiAiTrainingTapeMemoryInspect(tape, &before) == UMI_STATUS_OK);
            CHECK(UmiAiTrainingMaskedMeanSquaredError(tape, foreign, target, mask, 6U, &loss) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(loss == prediction && UmiAiTrainingTapeMemoryInspect(tape, &after) == UMI_STATUS_OK);
            CHECK(!memcmp(&before, &after, sizeof before));
        } else if (!strcmp(mode, "shared-gradient")) {
            UmiAiTrainingTensor *first = NULL, *second = NULL, *sum = NULL;
            unsigned char one[6] = {1, 0, 0, 0, 0, 0}, two[6] = {0, 0, 1, 0, 0, 0};
            CHECK(UmiAiTrainingMaskedMeanSquaredError(tape, prediction, target, one, 6U, &first) == UMI_STATUS_OK);
            CHECK(UmiAiTrainingMaskedMeanSquaredError(tape, prediction, target, two, 6U, &second) == UMI_STATUS_OK);
            CHECK(UmiAiTrainingAdd(tape, first, second, &sum) == UMI_STATUS_OK);
            CHECK(UmiAiTrainingBackward(tape, sum) == UMI_STATUS_OK);
            CHECK(UmiAiTrainingTensorGradients(prediction, pg, 6U) == UMI_STATUS_OK);
            CHECK(pg[0] == 2 && pg[2] == 18 && pg[1] == 0 && pg[5] == 0);
            CHECK(UmiAiTrainingTensorGradients(target, tg, 6U) == UMI_STATUS_OK);
            for (size_t i = 0U; i < 6U; ++i) CHECK(tg[i] == -pg[i]);
        } else if (!strcmp(mode, "same-input")) {
            CHECK(UmiAiTrainingMaskedMeanSquaredError(tape, prediction, prediction, mask, 6U, &loss) == UMI_STATUS_OK);
            CHECK(UmiAiTrainingTensorValues(loss, &value, 1U) == UMI_STATUS_OK && value == 0);
            CHECK(UmiAiTrainingBackward(tape, loss) == UMI_STATUS_OK);
            CHECK(UmiAiTrainingTensorGradients(prediction, pg, 6U) == UMI_STATUS_OK);
            for (size_t i = 0U; i < 6U; ++i) CHECK(pg[i] == 0);
        } else if (!strcmp(mode, "rollback")) {
            size_t checkpoint = 0U;
            CHECK(UmiAiTrainingTapeCheckpoint(tape, &checkpoint) == UMI_STATUS_OK && checkpoint == 2U);
            for (size_t i = 0U; i < 20U; ++i) {
                CHECK(UmiAiTrainingMaskedMeanSquaredError(tape, prediction, target, mask, 6U, &loss) == UMI_STATUS_OK);
                CHECK(UmiAiTrainingBackward(tape, loss) == UMI_STATUS_OK);
                CHECK(UmiAiTrainingTapeRollback(tape, checkpoint) == UMI_STATUS_OK);
                CHECK(UmiAiTrainingTapeMemoryInspect(tape, &after) == UMI_STATUS_OK);
                CHECK(after.currentBytes == before.currentBytes && after.auxiliaryBytes == before.auxiliaryBytes);
            }
        } else if (!strcmp(mode, "node-capacity")) {
            const float zero = 0;
            for (size_t i = 2U; i < UMI_AI_TRAINING_MAX_TENSORS; ++i)
                CHECK(UmiAiTrainingTensorCreate(tape, 1U, 1U, &zero, 1U, 0, &foreign) == UMI_STATUS_OK);
            CHECK(UmiAiTrainingTapeMemoryInspect(tape, &before) == UMI_STATUS_OK);
            CHECK(UmiAiTrainingMaskedMeanSquaredError(tape, prediction, target, mask, 6U, &loss) == UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(loss == prediction && UmiAiTrainingTapeMemoryInspect(tape, &after) == UMI_STATUS_OK);
            CHECK(!memcmp(&before, &after, sizeof before));
        } else { UmiAiTrainingTapeDestroy(tape); return 2; }
        UmiAiTrainingTapeDestroy(other); UmiAiTrainingTapeDestroy(tape);
    }
    return 0;
}
